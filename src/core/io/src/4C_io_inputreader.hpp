// This file is part of 4C multiphysics licensed under the
// GNU Lesser General Public License v3.0 or later.
//
// See the LICENSE.md file in the top-level for license information.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FOUR_C_IO_INPUTREADER_HPP
#define FOUR_C_IO_INPUTREADER_HPP

#include "4C_config.hpp"

#include "4C_comm_pack_buffer.hpp"
#include "4C_fem_discretization.hpp"
#include "4C_utils_parameter_list.fwd.hpp"
#include "4C_utils_std_cxx20_ranges.hpp"
#include "4C_utils_string.hpp"

#include <Epetra_Comm.h>
#include <Teuchos_RCP.hpp>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <variant>

FOUR_C_NAMESPACE_OPEN

namespace Core::FE
{
  namespace Nurbs
  {
    class Knotvector;
  }
}  // namespace Core::FE

namespace Core::IO
{
  namespace Internal
  {
    /**
     * An internal iterator that walks over lines in a stream. This iterator is used as an
     * ingredient for the DatFileLineIterator.
     */
    class StreamLineIterator
    {
     public:
      using iterator_category = std::input_iterator_tag;
      using value_type = std::string_view;
      using pointer = const value_type*;
      using reference = value_type;
      using difference_type = int;

      /**
       * Read lines from the given @p stream.
       */
      explicit StreamLineIterator(std::shared_ptr<std::istream> stream);

      /**
       * Read lines rom the given @p stream but a maximum of @p max_reads times. After reading
       * that often, the iterator will be considered past-the-end.
       *
       * @note @p max_reads may be zero. The iterator will not read any lines in that case.
       */
      StreamLineIterator(std::shared_ptr<std::istream> stream, int max_reads);

      /**
       * Construct a past-the-end iterator.
       */
      StreamLineIterator();

      StreamLineIterator& operator++();

      reference operator*() const;

      bool operator==(const StreamLineIterator& other) const;

      bool operator!=(const StreamLineIterator& other) const;

     private:
      //! Stream to read from.
      std::shared_ptr<std::istream> stream_{};

      //! Current line number.
      int line_number_{};

      //! Maximum number of lines to read.
      int max_reads_{};

      //! Currently read line.
      std::string line_;
    };

    /**
     * An internal iterator that walks over lines in a dat file. Providing
     * a custom iterator allows us to abstract away the differences between
     * reading from a file directly or querying lines from a pre-read vector.
     */
    class DatFileLineIterator
    {
     public:
      using PreReadIterator = std::vector<std::string_view>::const_iterator;
      using iterator_category = StreamLineIterator::iterator_category;
      using value_type = StreamLineIterator::value_type;
      using pointer = StreamLineIterator::pointer;
      using reference = StreamLineIterator::reference;
      using difference_type = StreamLineIterator::difference_type;

      //! Construct an iterator that reads from the given stream.
      explicit DatFileLineIterator(std::variant<StreamLineIterator, PreReadIterator> iterator);

      DatFileLineIterator& operator++();

      reference operator*() const;

      bool operator==(const DatFileLineIterator& other) const;

      bool operator!=(const DatFileLineIterator& other) const;

     private:
      //! The two ways to iterate over dat file content are handled by the iterator variants.
      std::variant<StreamLineIterator, PreReadIterator> iterator_;
    };
  }  // namespace Internal

  /*----------------------------------------------------------------------*/
  /// reading, broadcasting and storing of dat file contents
  /*!
    The normal 4C job needs to read one dat file at the beginning of
    its execution. This file contains all input parameters as well as
    the meshes and so on and needs to be available on all
    processors. However, we cannot rely on a shared file system, so only
    on process (the first one) can do the actual reading. All others
    need to get the data via MPI communication.

    This class manages the reading and broadcasting of the input
    file. To do so processor 0 reads the input file line by line and
    stores all lines in an array. Afterwards these lines are
    communicated to all other processors, so each processor has an
    internal copy of the input file and subsequently parses it.  There
    is one instance of this class during the input phase of 4C. When
    everything is set up properly this instance goes away and the
    internal copy of the input file vanishes.

    There is one more details to be considered: we do not want to
    read all elements and nodes on each processor. So we skip reading
    these sections here. Elements and nodes are read in parallel by the
    ElementReader and MeshReader, respectively.

   */
  /*----------------------------------------------------------------------*/
  class DatFileReader
  {
   public:
    /// construct a reader for a given file
    DatFileReader(std::string filename, const Epetra_Comm& comm, int outflag = 0);

    /// return my inputfile name
    [[nodiscard]] std::string my_inputfile_name() const;

    /// return my output flag
    [[nodiscard]] int my_output_flag() const;

    /**
     * Get a a range of lines inside a section that have actual content, i.e., they contain
     * something other than whitespace or comments. Any line returned will have comment stripped
     * and whitespace trimmed. The usual way to do something with the lines is
     *
     * @code
     *   for (const auto& line : reader.lines_in_section("section_name"))
     *   {
     *     // do something with line
     *   }
     * @endcode
     *
     * Depending on the @p section_name, the lines were either pre-read or are read from the file
     * on-the-fly.
     *
     * @return A range of string_views to the lines in this section.
     */
    auto lines_in_section(const std::string& section_name);

    /**
     * Returns whether a section with the given name exists in the input file and contains any
     * content.
     */
    [[nodiscard]] bool has_section(const std::string& section_name) const;

    /**
     * Access MPI communicator associated with this object.
     */
    [[nodiscard]] const Epetra_Comm& get_comm() const { return comm_; }

    /**
     * Print a list of all sections that are contained in the input file but never
     * accessed through this object.
     *
     * @return True if there were unknown sections, false otherwise.
     */
    bool print_unknown_sections(std::ostream& out) const;

   private:
    /// actual read dat file, store and broadcast general sections
    void read_dat();

    //! Remember that a section was used.
    void record_section_used(const std::string& section_name);

    //! Internal helper to get the range of lines in a section.
    //! Does not record the section as used.
    [[nodiscard]] auto line_range(const std::string& section_name) const;

    //! Helper to store section positions.
    struct SectionPosition
    {
      std::filesystem::path file;
      std::ifstream::pos_type pos;
      unsigned int length;

      void pack(Core::Communication::PackBuffer& data) const;

      void unpack(Core::Communication::UnpackBuffer& buffer);
    };

    /// The top-level file that is first read by this object.
    std::filesystem::path top_level_file_;

    /// The communicator associated with this object.
    const Epetra_Comm& comm_;

    /// Flag for output (default: output should be written)
    int outflag_{};

    /// The whole input file.
    std::vector<char> inputfile_;

    /// The lines of the #input_file_ as std::string_view.
    std::vector<std::string_view> lines_;

    /// file positions of skipped sections
    std::map<std::string, SectionPosition> excludepositions_;

    /// Section positions of all sections inside the #inputfile_ array.
    std::map<std::string, std::pair<std::size_t, std::size_t>> positions_;

    /// Protocol of known and unknown section names
    std::map<std::string, bool> knownsections_;
  };

  /**
   * Split the given @p line into a key-value pair. Key and value are normally separated by
   * whitespace. In case there are multiple distinct whitespace groups in one line, the first of
   * these is assumed to be the separator and all the other whitespace is assumed to be part of
   * the value. Key and value may also be separated by an equals sign "=" and at least one
   * whitespace character on both sides. In this case, key and value may contain spaces
   * internally. Leading and trailing whitespace is trimmed from both key and value.
   *
   * @throws Core::Exception If the @p line cannot be read.
   *
   * @return A pair of key and value.
   */
  std::pair<std::string, std::string> read_key_value(const std::string& line);

  /**
   * Read a @p section_name from the input file and store the key-value pairs in the given @p list.
   */
  bool read_parameters_in_section(
      DatFileReader& reader, const std::string& section_name, Teuchos::ParameterList& list);

  /**
   * Read a node-design topology section
   *
   * @param reader The dat file reader
   * @param name Name of the topology to read
   * @param dobj_fenode Resulting collection of all nodes that belong to a design.
   * @param get_discretization Callback to return a discretization by name.
   */
  void read_design(DatFileReader& reader, const std::string& name,
      std::vector<std::vector<int>>& dobj_fenode,
      const std::function<const Core::FE::Discretization&(const std::string& name)>&
          get_discretization);

  /**
   * \brief read the knotvector section (for isogeometric analysis)
   *
   * \param  reader         (in ): DatFileReader object
   * \param  name           (in ): Name/type of discretisation
   * \param  disknots       (out): node vector coordinates
   *
   */
  void read_knots(DatFileReader& reader, const std::string& name,
      Teuchos::RCP<Core::FE::Nurbs::Knotvector>& disknots);


  /// -- template and inline functions --- //

  inline auto DatFileReader::line_range(const std::string& section_name) const
  {
    auto filter = [](std::string_view line)
    { return !Core::Utils::strip_comment(std::string(line)).empty(); };

    if (excludepositions_.count(section_name) > 0)
    {
      const auto [path, start_pos, length] = excludepositions_.at(section_name);

      auto file = std::make_shared<std::ifstream>(path);
      file->seekg(start_pos);

      return std_20::ranges::views::filter(
          std_20::ranges::views::Internal::IteratorRange(
              Internal::DatFileLineIterator(Internal::StreamLineIterator(std::move(file), length)),
              Internal::DatFileLineIterator(Internal::StreamLineIterator())),
          filter);
    }

    const auto [start_line, end_line] = std::invoke(
        [&]() -> std::pair<std::size_t, std::size_t>
        {
          auto entry_it = positions_.find(section_name);
          if (entry_it == positions_.end()) return {lines_.size(), lines_.size()};
          return entry_it->second;
        });

    return std_20::ranges::views::filter(
        std_20::ranges::views::Internal::IteratorRange(
            Internal::DatFileLineIterator(lines_.begin() + start_line),
            Internal::DatFileLineIterator(lines_.begin() + end_line)),
        filter);
  }

  inline auto DatFileReader::lines_in_section(const std::string& section_name)
  {
    record_section_used(section_name);
    return line_range(section_name);
  }


}  // namespace Core::IO

FOUR_C_NAMESPACE_CLOSE

#endif
