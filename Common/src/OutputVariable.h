/*	
	NovaGenesis
	
	Name:		OutputVariable
	Object:		OutputVariable
	File:		OutputVariable.h
	Author:		Antonio Marcos Alberti
	Date:		05/2021
	Version:	0.4

 	Copyright (C) 2021  Antonio Marcos Alberti

    This work is available under the GNU Lesser General Public License (See COPYING.txt).

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef __OUTPUTVARIABLE_H
#define __OUTPUTVARIABLE_H

#ifndef _FILE_H
#include "File.h"
#endif

#define ERROR 1
#define OK 0

using namespace std;

class Block;

class OutputVariable {
 private:

  // To be exposed to users
  string Name;
  string Type;
  string Description;

  // Internal use only
  double Value;
  double S1;
  double t_1;
  double v_1;
  double Mean;                // Sample Mean
  double Sqr;
  double S2;
  double Sigma;                // Sample Standard Deviation
  double SE;                    // Standard Error
  double ME;                    // Margin of Error
  double Lower;                // Mean - Margin of Error
  double Up;                    // Mean + Margin of Error
  unsigned int Samples_Number;
  double Offset;

  // Interface
  Block *Owner;

  // Output file data
  File Results;
  int Option;

 public:

  OutputVariable (Block *Owner_);
  ~OutputVariable ();

  // Set functions
  void SetName (string Name_)
  { Name = Name_; }
  void SetType (string Type_)
  { Type = Type_; }
  void SetDescription (string Description_)
  { Description = Description_; }
  void SetOption (int Option_)
  { Option = Option_; }
  void Initialization (string Name_, string Type_, string Description_, int Option_);

  // Get functions
  string GetName ()
  { return Name; }
  string GetType ()
  { return Type; }
  string GetDescription ()
  { return Description; }
  int GetOption ()
  { return Option; }

  // Instantaneous value functions
  void Sample (double Value_)
  {
	Value = Value_;
	v_1 = Value;
  }
  double GetLastSample ()
  { return Value; }
  void SampleAsAnIncrement ()
  {
	Value++;
	v_1 = Value;
  }
  void SampleAsADecrement ()
  {
	Value--;
	v_1 = Value;
  }

  // Mean related functions
  void SetInitialMean (double InitialMean_)
  { Mean = InitialMean_; }
  void CalculateArithmetic ();
  void CalculateWeighted (double Time);
  double GetMean ()
  { return Mean; }

  // Owner related functions
  void SetOwner (Block *Owner_)
  { Owner = Owner_; }

  // Reset the statistics - continue to sample to file
  void Reset ();
  void ResetButKeepLastValue ();

  // Output file related functions
  void SetFileName (string File_Name_)
  { Results.SetName (File_Name_); }
  void SetFilePath (string File_Path_)
  { Results.SetPath (File_Path_); }
  string GetFileName ()
  { return Results.GetName (); }
  string GetFilePath ()
  { return Results.GetPath (); }

  // Sampling function
  void SampleToFile (double Time);
};

#endif
