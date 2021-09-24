/*	
	NovaGenesis
	
	Name:		OutputVariable
	Object:		OutputVariable
	File:		OutputVariable.cpp
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
#include "OutputVariable.h"
#endif

#ifndef _BLOCK_H
#include "Block.h"
#endif

#ifndef _PROCESS_H
#include "Process.h"
#endif

#include <math.h>

OutputVariable::OutputVariable (Block *Owner_)
{
  double t = Owner->GetTime ();

  Name = "UNKNOWN";
  Type = "UNKNOWN";
  Description = "UNKNOWN";
  S1 = 0;
  t_1 = t;
  v_1 = 0;
  Mean = 0;
  Sqr = 0;
  S2 = 0;
  Sigma = 0;
  SE = 0;
  ME = 0;
  Lower = 0;
  Up = 0;
  Samples_Number = 1;
  Value = 0;
  Owner = Owner_;
  Option = 0;
  Offset = t;
}

OutputVariable::~OutputVariable ()
{
}

void OutputVariable::Initialization (string Name_, string Type_, string Description_, int Option_)
{
  Name = Name_;
  Type = Type_;
  Description = Description_;
  Option = Option_;

  string FileName;

  FileName = Owner->PP->GetLegibleName () + "_" + Owner->GetLegibleName () + "_" + Name_ + ".dat";

  Results.SetName (FileName);
  Results.SetPath (Owner->GetPath ());
  Results.SetOption ("DEFAULT");
  Results.SetDescription ("");

  Results.OpenOutputFile (FileName, Owner->GetPath (), "DEFAULT");

  //Results<<"t"<<"\t"<<Name<<"\t"<<"M_"+Name<<endl;

  if (Option == 1)
	{
	  Results.CloseFile ();
	}

  Results.setf (ios::scientific);
}

void OutputVariable::CalculateArithmetic ()
{
  S1 = S1 + Value;
  Mean = S1 / Samples_Number;
  Sqr = pow ((Value - Mean), 2);
  S2 = S2 + Sqr;

  if (Samples_Number > 2)
	{
	  Sigma = sqrt (S2 / (Samples_Number - 1));
	  SE = Sigma / sqrt (Samples_Number);
	  ME = 1.96 * SE;
	  Lower = Mean - ME;
	  Up = Mean + ME;
	}

  //if (Name == "twi")
  //	{
  //		cout << setprecision(10) << endl << endl << "Value = " << Value << endl;
  //		cout << setprecision(10) << "S1 = " << S1 << endl;
  //		cout << setprecision(10) << "Mean = " << Mean << endl;
  //		cout << setprecision(10) << "Sqr = " << Sqr << endl;
  //		cout << setprecision(10) << "S2 = " << S2 << endl;
  //		cout << setprecision(10) << "Samples_Number = " << (Samples_Number) << endl;
  //		cout << setprecision(10) << "Sigma = " << Sigma << endl;
  //		cout << setprecision(10) << "SE = " << SE << endl;
  //		cout << setprecision(10) << "ME = " << ME << endl;
  //		cout << setprecision(10) << "Lower = " << Lower << endl;
  //		cout << setprecision(10) << "Up = " << Up << endl;
  //	}

  Samples_Number++;
}

void OutputVariable::CalculateWeighted (double Time)
{
  double E = 0;

  if (Time == 0) Time = 1e-150;
  if (Time == Offset) Time = Offset + 1e-150;

  E = (Time - t_1) * v_1;

  S1 = S1 + E;
  Mean = S1 / (Time - Offset);

  Sqr = pow ((v_1 - Mean), 2);
  S2 = S2 + Sqr;

  if (Samples_Number > 2)
	{
	  Sigma = sqrt (S2 / (Samples_Number - 1));
	  SE = Sigma / sqrt (Samples_Number);
	  ME = 1.96 * SE;
	  Lower = Mean - ME;
	  Up = Mean + ME;
	}

  //if (Name == "wi")
  //	{
  //		cout << setprecision(10) << endl << endl << "Time = " << Time << endl;
  //		cout << setprecision(10) << "t_1 = " << t_1 << endl;
  //		cout << setprecision(10) << "Delta t = " <<(Time-t_1)<< endl;
  //		cout << setprecision(10) << "v_1 = " << v_1 << endl;
  //		cout << setprecision(10) << "Area (E) = " <<E<< endl;
  //		cout << setprecision(10) << "S1 = " << S1 << endl;
  //		cout << setprecision(10) << "Time - Offset = " << (Time - Offset) << endl;
  //		cout << setprecision(10) << "Mean = " << Mean << endl;
  //		cout << setprecision(10) << "Sqr = " << Sqr << endl;
  //		cout << setprecision(10) << "S2 = " << S2 << endl;
  //		cout << setprecision(10) << "Samples_Number = " << (Samples_Number) << endl;
  //		cout << setprecision(10) << "Sigma = " << Sigma << endl;
  //		cout << setprecision(10) << "SE = " << SE << endl;
  //		cout << setprecision(10) << "ME = " << ME << endl;
  //		cout << setprecision(10) << "Lower = " << Lower << endl;
  //		cout << setprecision(10) << "Up = " << Up << endl;
  //	}

  t_1 = Time;
  Samples_Number++;
}

void OutputVariable::Reset ()
{
  double t = Owner->GetTime ();

  S1 = 0;
  t_1 = t;
  v_1 = 0;
  Mean = 0;
  Sqr = 0;
  S2 = 0;
  Sigma = 0;
  SE = 0;
  ME = 0;
  Lower = 0;
  Up = 0;
  Samples_Number = 1;
  Value = 0;
  Offset = t;
}

void OutputVariable::ResetButKeepLastValue ()
{
  double t = Owner->GetTime ();

  S1 = 0;
  t_1 = t;
  v_1 = 0;
  Mean = 0;
  Sqr = 0;
  S2 = 0;
  Sigma = 0;
  SE = 0;
  ME = 0;
  Lower = 0;
  Up = 0;
  Samples_Number = 1;
  Offset = t;
}

void OutputVariable::SampleToFile (double Time)
{
  // Sampling
  if (Option == 1)
	{
	  Results.OpenOutputFile ();
	}

  Results << setprecision (10) << Time << " " << Value << " ";

  if (Type == "MEAN_WEIGHTED")
	{
	  if (Time > t_1)
		{
		  CalculateWeighted (Time);
		}
	}

  Results << setprecision (10) << Mean << " " << Sigma << " " << SE << " " << ME << " " << Lower << " " << Up << endl;

  if (Option == 1)
	{
	  Results.CloseFile ();
	}
}
