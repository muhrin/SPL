set(sslib_src_Source_Files__build_cell
  build_cell/AbstractConstraintDescription.cpp
  build_cell/AtomConstraintDescription.cpp
  build_cell/AtomGroupDescription.cpp
  build_cell/AtomsDescription.cpp
  build_cell/CellConstraintDescription.cpp
  build_cell/DefaultCrystalGenerator.cpp
  build_cell/Minsep.cpp
  build_cell/StructureBuilder.cpp
  build_cell/StructureConstraintDescription.cpp
  build_cell/StructureDescription.cpp
)
source_group("Source Files\\build_cell" FILES ${sslib_src_Source_Files__build_cell})

set(sslib_src_Source_Files__common
  common/Atom.cpp
  common/AtomGroup.cpp
  common/AtomSpeciesDatabase.cpp
  common/AtomSpeciesInfo.cpp
  common/Constants.cpp
  common/Structure.cpp
  common/StructureTreeEvent.cpp
  common/UnitCellFactory.cpp
  common/Utils.cpp
)
source_group("Source Files\\common" FILES ${sslib_src_Source_Files__common})

set(sslib_src_Source_Files__common__event
  common/event/AtomGroupEvent.cpp
  common/event/PropertyChangeEvent.cpp
  common/event/PropertyChangeSupport.cpp
)
source_group("Source Files\\common\\event" FILES ${sslib_src_Source_Files__common__event})

set(sslib_src_Source_Files__io
  io/AdditionalData.cpp
  io/ResReaderWriter.cpp
  io/StructureWriterManager.cpp
)
source_group("Source Files\\io" FILES ${sslib_src_Source_Files__io})

set(sslib_src_Source_Files__utility
  utility/SortedDistanceComparator.cpp
)
source_group("Source Files\\utility" FILES ${sslib_src_Source_Files__utility})

set(sslib_src
  SSLibConfig.h
  SSLib.h
  ${sslib_src_Source_Files__build_cell}
  ${sslib_src_Source_Files__common}
  ${sslib_src_Source_Files__common__event}
  ${sslib_src_Source_Files__io}
  ${sslib_src_Source_Files__utility}
)


add_library(sslib SHARED ${sslib_inc} ${sslib_src})