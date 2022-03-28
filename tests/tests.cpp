#define CATCH_CONFIG_MAIN
#include "catch2.hpp"

#include <cstdint>
#include <array>


#include "hello.h"// Header file with program content as an std::array (for PE)
#include "hello_elf.h"// header file with program content for ELF

#include "pelfParser.h"


namespace hana = boost::hana;

/*
  TODO:
    Add many more tests :(


*/

inline constexpr auto sections = pelf::getPeNumberOfSections(hello_program);

constexpr pelf::Pe<decltype(hello_program), sections> compile_pe{
  hello_program
};


pelf::Pe runtime_pe{ hello_program };


inline constexpr auto elf_tables_size =
  pelf::Elf<decltype(hello_program_elf)>::getElfTablesSize(hello_program_elf);


constexpr pelf::Elf<decltype(hello_program_elf),
  elf_tables_size.sectionTable,
  elf_tables_size.programTable>
  compile_elf{ hello_program_elf };


TEST_CASE("Test getRawData()")
{


  static constexpr auto data = compile_pe.getRawData();

  REQUIRE(data == hello_program);
}


TEST_CASE("Test constexpr coff header parsing")
{


  static constexpr auto coff = compile_pe.getHeaders().getCoffHeader();

  static constexpr auto machine = coff.Machine;
  static constexpr auto number_of_sections = coff.NumberOfSections;
  static constexpr auto number_of_symbols = coff.NumberOfSymbols;


  REQUIRE(machine == 0x8664);
  REQUIRE(number_of_sections == 0x6);
  REQUIRE(number_of_symbols == 0x00);
}


TEST_CASE("Test runtime coff header parsing")
{

  auto coff = runtime_pe.getHeaders().getCoffHeader();
  auto machine = coff.Machine;
  auto number_of_sections = coff.NumberOfSections;
  auto number_of_symbols = coff.NumberOfSymbols;

  REQUIRE(machine == 0x8664);
  REQUIRE(number_of_sections == 0x6);
  REQUIRE(number_of_symbols == 0x00);
}

TEST_CASE("Test equality between coff header at compile time and run time")
{

  static constexpr auto compile_coff = compile_pe.getHeaders().getCoffHeader();
  auto runtime_coff = runtime_pe.getHeaders().getCoffHeader();

  const pelf::IMAGE_FILE_HEADER empty_coff{};

  BOOST_HANA_RUNTIME_CHECK(hana::not_equal(compile_coff, empty_coff));

  BOOST_HANA_RUNTIME_CHECK(hana::equal(compile_coff, runtime_coff));
}


TEST_CASE("Test Standard Coff Fields")
{
  static constexpr auto compile_scf =
    compile_pe.getHeaders().getStandardCoffFields();
  auto runtime_scf = runtime_pe.getHeaders().getStandardCoffFields();

  BOOST_HANA_RUNTIME_CHECK(hana::equal(compile_scf, runtime_scf));


  REQUIRE(compile_scf.Magic == 0x20b);
  REQUIRE(compile_scf.AddressOfEntryPoint == 0x47b0);
}


TEST_CASE("Test Windows Specific Fields")
{

  static constexpr auto compile_wsf =
    compile_pe.getHeaders().getWindowsSpecificFields();
  auto runtime_wsf = runtime_pe.getHeaders().getWindowsSpecificFields();

  BOOST_HANA_RUNTIME_CHECK(hana::equal(compile_wsf, runtime_wsf));

  REQUIRE(compile_wsf.ImageBase == 0x140000000);
}


TEST_CASE("Test Pe sections")
{

  static constexpr auto compile_sections = compile_pe.getSections();
  static constexpr auto compile_coff = compile_pe.getHeaders().mCoffHeader;
  const auto runtime_sections = runtime_pe.getSections();


  REQUIRE(compile_coff.NumberOfSections == compile_sections.size());

  REQUIRE(runtime_sections.size() == compile_sections.size());
}


TEST_CASE("Test ELF table section size and program header table size")
{

  REQUIRE(elf_tables_size.programTable == 11);
  REQUIRE(elf_tables_size.sectionTable == 29);
}


TEST_CASE("Test Pe and ELF Constructor exceptions")
{
  REQUIRE_NOTHROW([&]() {
    pelf::Elf elf{ hello_program_elf };
    pelf::Pe pe{ hello_program };
  });

  /* Need to implement more custom exceptions like invalidSize,
   * invalidSignatures, etc */
  REQUIRE_THROWS_AS(pelf::Elf{ hello_program }, pelf::PelfException);

  REQUIRE_THROWS_AS(pelf::Pe{ hello_program_elf }, pelf::PelfException);
}

TEST_CASE("Test ELF header parsing")
{


  pelf::Elf runtime_elf{ hello_program_elf };

  static constexpr auto elf_header = compile_elf.getHeaders().elfHeader;

  static constexpr auto program_header =
    compile_elf.getHeaders().programHeaders;


  auto elf_header_runtime = runtime_elf.getHeaders().elfHeader;
  auto program_header_runtime = runtime_elf.getHeaders().programHeaders;

  pelf::Elf64_Phdr program_header_test = {};

  /* Comparing equality between headers at compile time and runtime */
  BOOST_HANA_RUNTIME_CHECK(hana::equal(elf_header_runtime, elf_header));
  BOOST_HANA_RUNTIME_CHECK(
    hana::equal(program_header_runtime.at(0), program_header.at(0)));
  /* Checking is not an uninitialized header */
  BOOST_HANA_RUNTIME_CHECK(
    hana::not_equal(program_header_runtime.at(0), program_header_test));


  /* Check programHeaders size at compile and runtime */
  REQUIRE(program_header.size() == elf_tables_size.programTable);
  REQUIRE(program_header_runtime.size() == elf_tables_size.programTable);

  /* Checking entry point */
  REQUIRE(elf_header.e_entry == 0x4010c0);

  /* Checking table offsets */
  REQUIRE(elf_header.e_phoff == 64);
  REQUIRE(elf_header.e_shoff == 15496);
}