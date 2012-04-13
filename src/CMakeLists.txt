


set(sslib_src
  SSLibConfig.h
  SSLib.h
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
  common/Atom.cpp
  common/AtomGroup.cpp
  common/AtomSpeciesDatabase.cpp
  common/AtomSpeciesInfo.cpp
  common/Constants.cpp
  common/Structure.cpp
  common/StructureTreeEvent.cpp
  common/UnitCellFactory.cpp
  common/Utils.cpp
  common/event/AtomGroupEvent.cpp
  common/event/PropertyChangeEvent.cpp
  common/event/PropertyChangeSupport.cpp
  io/AdditionalData.cpp
  io/ResReaderWriter.cpp
  io/StructureWriterManager.cpp
  utility/SortedDistanceComparator.cpp
)

add_library(sslib shared ${sslib_src})