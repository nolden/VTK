/*=========================================================================

  Program:   Visualization Library
  Module:    CellList.hh
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

This file is part of the Visualization Library. No part of this file or its 
contents may be copied, reproduced or altered in any way without the express
written consent of the authors.

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen 1993, 1994 

=========================================================================*/
//
// Supplemental object to CellArray to allow random access into cells.
// The "location" field is the location in the CellArray list in terms of an 
// integer offset.  An integer offset was used instead of a pointer for easy 
// storage and inter-process communication.
//
#ifndef __vlCellList_h
#define __vlCellList_h

#include "Object.hh"

struct vlCell_s {
    unsigned char type; //from CellTypes.hh
    int loc; //location in associated CellArray object
};

class vlCellList : public vlObject {
public:
  vlCellList() : Array(NULL),Size(0),MaxId(-1),Extend(1000) {};
  vlCellList(const int sz, const int ext);
  ~vlCellList();
  char *GetClassName() {return "vlCellList";};
  vlCell_s &GetCell(const int id) {return this->Array[id];};
  unsigned char GetCellType(const int id) {return this->Array[id].type;};
  int GetCellLocation(const int id) {return this->Array[id].loc;};
  void InsertCell(const int id, const unsigned char type, const int loc);
  int InsertNextCell(const unsigned char type, const int loc);
  void Squeeze();
  void Reset();

private:
  vlCell_s *Array;   // pointer to data
  int Size;       // allocated size of data
  int MaxId;     // maximum index inserted thus far
  int Extend;     // grow array by this point
  vlCell_s *Resize(const int sz);  // function to resize data
};

#endif
