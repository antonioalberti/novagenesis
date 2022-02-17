#ifndef _FILE_H
#include "File.h"
#endif

File::File()
   :fstream()
{
}

File::File(const File& F)
{
	Name=F.GetName();
	Option=F.GetOption();
	Description=F.GetDescription();
}

File& File::operator=(const File &F)
{
	Name=F.GetName();
	Option=F.GetOption();
	Description=F.GetDescription();

	return *this;
}

int File::OpenOutputFile(string Name_, string Option_)
{
	if (FileIsOpen() == 1) // se for igual a 1 o arquivo esta aberto
   		{
			return OK;
		}
	else
   		{
      		if (Option_ == "BINARY")
         		{
					open(Name_.c_str(),ios::out|ios::binary);

					if (ios::fail() == 1) // se for igual a 1 houve erro
		         		{
      						return ERROR;
						}
					else
		         		{
		            		Name=Name_;
							Option=Option_;
							
							return OK;
						}
				}

			if (Option_ == "DEFAULT")
         		{
					open(Name_.c_str(),ios::out);

					if (ios::fail() == 1) // se for igual a 1 houve erro
         				{														      	      				
							return ERROR;
	   					}
					else
      		   			{
                  			Name=Name_;
							Option=Option_;
																			
							return OK;
	         			}
				}
			else
				{
					return ERROR;
				}
		}
}

int File::OpenOutputFile()
{
	if (FileIsOpen() == 1) // se for igual a 1 o arquivo esta aberto
   		{						
			return ERROR;
		}
	else
   		{
      		if (Option == "BINARY")
         		{
					open(Name.c_str(),ios::out|ios::app|ios::binary);

					if (ios::fail() == 1) // se for igual a 1 houve erro
         				{
      	      				return ERROR;
	   					}
					else
      		   			{
   	   						return OK;
	         			}
				}

			if (Option == "DEFAULT")
         		{
					open(Name.c_str(),ios::out|ios::app);
				
					if (ios::fail() == 1) // se for igual a 1 houve erro
         				{											
      	      				return ERROR;
	   					}
					else
      		   			{													
   	   						return OK;
	         			}
				}
			else
				{
					return ERROR;
				}
      }
}

int File::OverwriteOutputFile()
{
	if (FileIsOpen() == 1) // se for igual a 1 o arquivo esta aberto
   		{						
			return OK;
		}
	else
   		{
      		if (Option == "BINARY")
         		{
					open(Name.c_str(),ios::out|ios::binary);

					if (ios::fail() == 1) // se for igual a 1 houve erro
         				{
      	      				return ERROR;
	   					}
					else
      		   			{
   	   						return OK;
	         			}
				}

			if (Option == "DEFAULT")
         		{
					open(Name.c_str(),ios::out);
				
					if (ios::fail() == 1) // se for igual a 1 houve erro
         				{											
      	      				return ERROR;
	   					}
					else
      		   			{													
   	   						return OK;
	         			}
				}
			else
				{
					return ERROR;
				}
      }
}

int File::OpenInputFile(string Name_, string Option_)
{
	if (FileIsOpen() == 1) // se for igual a 1 o arquivo esta aberto
   		{
			return OK;
		}
	else
   		{
      		if (Option_ == "BINARY")
         		{
					open(Name_.c_str(),ios::in|ios::binary|ios::ate);

					if (ios::fail() == 1) // se for igual a 1 houve erro
		         		{
      						return ERROR;
						}
					else
		         		{
		            		Name=Name_;
							Option=Option_;
						
							return OK;
						}
				}

        	if (Option_ == "DEFAULT")
     			{
					open(Name_.c_str(),ios::in);

					if (ios::fail() == 1) // se for igual a 1 houve erro
	        			{					
	     	      			return ERROR;
	   					}
					else
	     		   		{
                  			Name=Name_;
							Option=Option_;
							
							return OK;
						}
	         	}
			else
				{
					return ERROR;
				}
		}
}

int File::OpenInputFile()
{
	if (FileIsOpen() == 1) // se for igual a 1 o arquivo esta aberto
   		{
			return OK;
		}
	else
   		{
      		if (Option == "BINARY")
         		{
					open(Name.c_str(),ios::in|ios::binary|ios::ate);

					if (ios::fail() == 1) // se for igual a 1 houve erro
         				{
      	      				return ERROR;
	   					}
					else
      		   			{
   	   						return OK;
	         			}
				}

			if (Option == "DEFAULT")
         		{
					open(Name.c_str(),ios::in);

					if (ios::fail() == 1) // se for igual a 1 houve erro
         				{				   
      	      				return ERROR;
	   					}
					else
      		   			{
   	   						return OK;
	         			}
				}
			else
				{
					return ERROR;
				}
		}
}

int File::CloseFile()
{
	if (FileIsOpen() == 1) // Retorna 1 se o arquivo esta aberto
   		{
			close();

			return OK;
		}

   return ERROR;
}

int File::FileIsOpen()
{
	return rdbuf()->is_open();  // Retorna 1 se o arquivo esta aberto
}

int File::CheckForFileFail()
{
	return ios::fail();
}

int File::CheckForEndOfFile()
{
	return ios::eof();
}
