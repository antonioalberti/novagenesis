/*
	NovaGenesis

	Name:		CommandLine
	Object:		CommandLine
	File:		CommandLine.cpp
	Author:		Antonio Marcos Alberti
	Date:		05/2021
	Version:	0.1

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

#ifndef _COMMANDLINE_H
#include "CommandLine.h"
#endif

CommandLine::CommandLine()
{
	Name = "";
	Alternative="";
	Version="";

	NoA=0;
	Arguments=0;
	NoE=0;
}

CommandLine::CommandLine(string _Name, string _Alternative, string _Version)
{
	Name = _Name;
	Alternative=_Alternative;
	Version=_Version;

	NoA=0;
	Arguments=0;
	NoE=0;
}

CommandLine::CommandLine(const CommandLine &CL)
{
	Name = CL.Name;
	Alternative=CL.Alternative;
	Version=CL.Version;

	NoA = CL.NoA;

	// Allocating the arguments
	Arguments = new string*[NoA];

	NoE=new unsigned int[NoA];

	// Copy the previous NoE array
	for (unsigned int m=0; m<NoA; m++)
		{
			NoE[m]=CL.NoE[m];

			Arguments[m]=new string[NoE[m]];
		}

	// Copying the elements
	for (unsigned int n=0; n<CL.NoA; n++)
		{
			for (unsigned int o=0; o<CL.NoE[n]; o++)
				{
					Arguments[n][o]=CL.Arguments[n][o];
				}
		}
}

CommandLine::~CommandLine()
{
	//cout << "It has "<<NoA<<" arguments)"<<endl;

	if (NoA > 0)
		{
			for (unsigned int i = 0; i<NoA; i++)
				{
					//cout << "(Going to delete the argument number "<<i<<")"<<endl;

					delete[] Arguments[i];
				}

			//cout << "(Going to delete the arguments array)"<<endl;

			delete[] Arguments;

			//cout << "(Going to delete the number of elements array)"<<endl;

			delete[] NoE;
		}
}

void CommandLine::NewArgument(int _Size)
{
	string** A = new string*[NoA+1];

	unsigned int *NE=new unsigned int[NoA+1];

	// Copy the previous NoE array
	for (unsigned int m=0; m<NoA; m++)
		{
			NE[m]=NoE[m];

			A[m]=new string[NE[m]];
		}

	// Copy the previous arguments elements
	for (unsigned int n=0; n<NoA; n++)
		{
			for (unsigned int o=0; o<NE[n]; o++)
				{
					A[n][o]=Arguments[n][o];
				}
		}

	// Set the new argument number of elements
	NE[NoA]=_Size;

	// Allocate the new argument
	A[NoA]=new string[NE[NoA]];

	if (NoA > 0)
		{
			// Delete the previous allocation
			for (unsigned int q = 0; q < NoA; ++q)
				{
					delete[] Arguments[q];
				}

			delete[] Arguments;

			delete[] NoE;
		}

	// Set the temporary array as the new arguments
	Arguments=A;

	// Set the temporary array of number of elements as the new array
	NoE=NE;

	// Update the NoA
	NoA++;

	//cout << "NoA = "<<NoA<<endl;
}

int CommandLine::GetArgument(unsigned int _Index, vector<string> & _Argument)
{
	if (_Index < NoA)
		{
			for (unsigned int i=0; i<NoE[_Index]; i++)
				{
					_Argument.push_back(Arguments[_Index][i]);
				}

			return OK;
		}

	return ERROR;
}

int CommandLine::GetNumberofArguments(unsigned int &_Number)
{
	_Number=NoA;

	return OK;
}

int CommandLine::GetNumberofArgumentElements(unsigned int _Index, unsigned int &_Number)
{
	if (_Index < NoA)
		{
			_Number=NoE[_Index];

			return OK;
		}

	return ERROR;
}

int CommandLine::SetArgumentElement(unsigned int _Index, unsigned int _Element, string _Value)
{
	if (_Index < NoA)
		{
			if (_Element < NoE[_Index])
				{
					Arguments[_Index][_Element]=_Value;

					return OK;
				}
			else
				{
					return ERROR;
				}
		}

	return ERROR;
}

int CommandLine::GetArgumentElement(unsigned int _Index, unsigned int _Element, string &_Value) const
{
	if (_Index < NoA)
		{
			if (_Element < NoE[_Index])
				{
					_Value=Arguments[_Index][_Element];

					return OK;
				}

			return ERROR;
		}

	return ERROR;
}

ostream&  operator<< (ostream& os, const CommandLine& CL)
{
	string 			NG="ng ";
	string 			SP=" ";
	string 			NL="\n";
	string 			OA="[ ";
	string 			CA="]";
	string 			OV="< ";
	string 			CV=">";
	unsigned int    _Size;

	os << NG << CL.Name << SP << CL.Alternative << SP << CL.Version;

	if(CL.NoA > 0)
		{
			os << SP << OA;

			// Run over the number of arguments
			for (unsigned int i=0; i< CL.NoA; i++)
				{
					os << OV;

					stringstream 	out;

					_Size=CL.NoE[i];

					//cout << endl <<"(The command line has "<<_Size<<" arguments)"<< endl;

					out << _Size;

					os << out.str() << SP << "s ";

					//cout << endl <<"(The argument has "<<PV1->size()<<" elements)"<< endl;

					for (unsigned int j=0; j<CL.NoE[i]; j++)
						{
							string E;

							E=CL.Arguments[i][j];

							os<< E << SP;
						}

					os << CV << SP;
				}

			os << CA << NL;
		}

	if(CL.NoA == 0)
		{
			os << NL;
		}

	return os;
}

istringstream& operator>> (istringstream& iss, CommandLine& CL)
{
	string			Temp;
	int				Size;
	unsigned int	Argument_Number=0;

	iss >> Temp;

	if (Temp == "ng")
		{
			iss >> CL.Name;
			iss >> CL.Alternative;
			iss >> CL.Version;
			iss >> Temp;              // Reads the [

			if (Temp == "[" && CL.Name != "" && CL.Alternative != "" && CL.Version != "" && Temp != "[<" )
				{
					while(!iss.eof())
						{
							iss >> Temp; // Reads the <

							if (Temp == "<" && Temp != "<1" && Temp != "<2" && Temp != "<3" && Temp != "]")
								{
									iss >> Temp; // Reads the number

									stringstream ssout(Temp.c_str());

									ssout>>Size;

									//cout << endl << "Size = "<<Size<<endl;

									if (Size > 0)
										{
											// New argument
											CL.NewArgument(Size);

											iss >> Temp; // Reads the type

											if ((Temp == "s" || Temp == "h" || Temp == "i") && (Temp != "1s" && Temp != "2s" && Temp != "3s") )
												{
													for (int i=0; i < Size; i++)
														{
															iss >> Temp; // Reads the value

															//cout << "Value = "<<Temp<<endl;

															if (Temp != "" && Temp != ">" && Temp != "<" && Temp != "]" && Temp != " ")
																{
																	// New element

																	CL.SetArgumentElement(Argument_Number,i,Temp);
																}
															else
																{
																	break;
																}
														}

													iss >> Temp; // Reads the >
												}
											else
												{
													break;
												}
										}
								}
							else
								{
									break;
								}

							Argument_Number++;
						}
				}
		}

	iss >> Temp; // Reads the ]

	//cout << "The command line is :" <<endl;

	//cout << CL << endl;

	return iss;
}

// Example of command line
// ng -p --notify _Version [ < 1 string _Category > < 1 string _Key > < _ValuesSize string S_1 ... S_ValuesSize > < _PubNotifySize string pub HID OSID PID BID > ... < _SubNotifySize string sub HID OSID PID BID > ]
int CommandLine::ConvertCommandLineFromCharArray(char *_CL, int _Size)
{
	bool 			HasNG=false;
	bool			HasCommandMarker=false;
	int				AlternativeMarkerPosition=0;
	bool			HasBeginArgumentMarker=false;
	bool			HasEndArgumentMarker=false;
	int				WhiteSpacePositions[4096]; 			// There is a limit of 4096 white space per command line
	int				WhiteSpaceCounter=0; 				// Number of white spaces detected
	int				BeginVectorMarkerCounter=0;      	// Zero means no marker
	int				EndVectorMarkerCounter=0;      		// Zero means no marker
	int 			Status=ERROR;

	for (int y=0; y<4096; y++)
		{
			WhiteSpacePositions[y]=0;
		}

	if (_Size > 9)
		{
			if (_CL[0] == 'n' && _CL[1] == 'g' && _CL[3] == '-')
				{
					HasNG=true;
					HasCommandMarker=true;

					// Loop over all characters
					for (int i=0; i<(_Size-1); i++)
						{
							if (_CL[i] == ' ')
								{
									WhiteSpacePositions[WhiteSpaceCounter]=i;
									WhiteSpaceCounter++;
								}

							if (_CL[i] == '-' && _CL[i+1] == '-')
								{
									AlternativeMarkerPosition=i;
								}

							if (_CL[i] == '[')
								{
									HasBeginArgumentMarker=true;
								}

							if (_CL[i+1] == ']')
								{
									HasEndArgumentMarker=true;
								}

							if (_CL[i] == '<')
								{
									BeginVectorMarkerCounter++;
								}

							if (_CL[i] == '>')
								{
									EndVectorMarkerCounter++;
								}

						}

					if (HasNG == true &&
					    HasCommandMarker == true &&
					    HasBeginArgumentMarker == true &&
					    HasEndArgumentMarker == true &&
					    AlternativeMarkerPosition > 0 &&
					    WhiteSpaceCounter > 3)
						{
							string *Words=new string[WhiteSpaceCounter+1];

							// Loop over white space positions
							for (int j=0; j<(WhiteSpaceCounter-1); j++)
								{
									//cout << "j = "<<j<<endl;
									//cout <<"White space at = "<<WhiteSpacePositions[j]<<endl;

									int Begin=WhiteSpacePositions[j];
									int	End=WhiteSpacePositions[j+1];

									if (End > Begin && Begin > 0)
										{
											for (int k=(Begin+1); k<End; k++)
												{
													Words[j]=Words[j]+_CL[k];

													//cout << "Temp["<<k<<"] = "<<_CL[k]<< endl;
												}

											//cout <<"I discovered the word = "<<Words[j]<<endl;
										}
								}

							//cout<<endl<<endl;

							for (int l=0; l<WhiteSpaceCounter+1; l++)
								{
									//cout <<Words[l]<<endl;

									if (l == 0) Name=Words[0];
									if (l == 1) Alternative=Words[1];
									if (l == 2) Version=Words[2];

									Status=OK;

									if (Words[l] == "<")
										{
											//cout <<endl<< "Vector Size = "<< Words[l+1]<<endl;
											//cout << "Vector Type = "<< Words[l+2]<<endl;

											stringstream ss(Words[l+1]);

											int VectorSize=0;

											ss >> VectorSize;

											//cout << "Integer Size = "<<VectorSize<<endl;

											if (VectorSize > 0 && VectorSize < 4096)				// The maximum number of elements in a vector
												{
													// New argument
													NewArgument(VectorSize);

													for (int m=0; m<VectorSize; m++)
														{
															int wi=l+3+m;

															if (wi <= WhiteSpaceCounter)
																{
																	//cout << "Value = "<<Words[wi]<<endl;

																	if (Words[wi] != "" && Words[wi] != ">" && Words[wi] != "<" && Words[wi] != "]" && Words[wi] != " ")
																		{
																			// New element
																			SetArgumentElement((NoA-1),m,Words[wi]);
																		}
																}
														}
												}
										}
								}

							delete[] Words;
						}
				}
		}

	return Status;
}

// Auxiliary functions
int CommandLine::StringToInt(string _String)
{
	stringstream ss(_String);

	int Temp=0;

	ss >> Temp;

	return Temp;
}
