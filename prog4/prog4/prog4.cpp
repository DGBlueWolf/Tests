/***************************************************************************//**
 * @file prog4.cpp
 *
 * @brief This program will make use of command line parameters, input and
 * output files and output manipulation. The program will read a file of 
 * data (up to 2000 lines) from an input file.  It will then make some 
 * calculations on given data and finally write results to an output file.
 *
 * @mainpage Program 4 - BMI Data Analysis
 *
 * @section course_section Course Information
 *
 * @authors Noah Brubaker
 *
 * @date April 26, 2013
 *
 * @par Instructor:
 *         Professor Manes
 *
 * @par Course:
 *         CSC 150 - M052 - 3:00 pm
 *
 * @par Location:
 *         Classroom Building - Room 203
 *
 * @section program_section Program Information
 *
 * @details This program will make use of command line parameters, input and
 * output files and output manipulation. The program will read in up to 2000
 * lines from a data file and do further calculations and analysis of the
 * collected data and finally write these results to a file.
 *
 * The program will read in a space delimited file of integer pairs, where
 * the first int is read as height and second as weight. It will calculate
 * and store the BMI (Body Mass Index) for each height/weight pair. It will 
 * then collect the BMIs into different bins according to given intervals.  
 * The program will then calculate the mean and standard deviation for the 
 * three data arrays (Height, Weight and BMI). Finally, the program creates 
 * a histogram of the BMI data and writes this to an output file together 
 * with a summary of the statistics. The program is intended to make use of
 * command line arguments taking the input filename and optionally the output
 * filename. The default output file is called: datagram.txt
 *
 * @section compile_section Compiling and Usage
 *
 * @par Compiling Instructions:
 *      None
 *
 * @par Usage:
   @verbatim
   c:\> prog4.exe (in.txt) [out.txt]
        (input filename), REQUIRED
        [output filename], optional

   @endverbatim
 *
 * @section todo_bugs_modification_section Todo, Bugs, and Modifications
 * 
 * 
 * @par Modifications and Development Timeline:
   @verbatim
   Date          Modification
   ------------  ---------------------------------------------------------------
   Apr  2, 2013  Copied/ pasted instructions and required function prototypes
   Apr  3, 2013  Wrote the read_in_data function and the calculation functions.
   Apr  4, 2013  Wrote the remaining functions and really focused on getting
                    the output file formatted correctly.
   Apr 22, 2013	 Learned about the use of command line arguments and implemented
                    this arguments in main. Also adjusted error output.
   Apr 25, 2013  Cleaned things up further and completed documentation and 
                    commenting.
   @endverbatim
 *
 ******************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <cmath>
using namespace std;

/*******************************************************************************
 *             Constant Variables, defines and Enums
 ******************************************************************************/
/*!
 * @brief Defines array size and is stopping point for the read function
 */
const int MAX = 2000;
/*******************************************************************************
 *             Array Indices
 ******************************************************************************/
/*!
 * @brief The index for height.
 */
const int Height = 0;
/*!
 * @brief The index for weight.
 */
const int Weight = 1;
/*!
 * @brief The index for BMI.
 */
const int BMI = 2;
/*!
 * @brief The index for the underweight bin.
 */
const int Underweight = 0;
/*!
 * @brief The index for the normal bin.
 */
const int Normal = 1;
/*!
 * @brief The index for overweight bin.
 */
const int Overweight = 2;
/*!
 * @brief The index for obese bin.
 */
const int Obese = 3;

/*******************************************************************************
 *                         Function Prototypes
 ******************************************************************************/
int read_in_data(const char filename[], double bmi_data[][MAX], int max);
void calculate_BMI(double bmi_data[][MAX], int bmi_group[], int num);
double calculate_mean(double data[], int num);
double calculate_stddev(double data[], double mean, int num);
bool write_histogram(const char filename[], int bmi_group[], int num);
void write_header(ofstream& fout, int num);
bool write_stats(const char filename[], char data_name[], 
	                 double mean, double stddev);
int find_max( int data[] , int size );

/***************************************************************************//**
 * @author Noah Brubaker
 *
 * @par Description:
 * This is the starting point of the program. It directs each function call
 * except for the write_header function and find_max function. Main also 
 * checks and interprets each value the functions return to check for an
 * error or continue on. Main will also take filenames from the commandline. 
 * It checks to make sure a proper number of arguments (argc) have been given 
 * and if not, displays a message showing proper usage of the program. If the 
 * arguments are correct, it passes the filenames ( from *argv[] ) to their
 * proper functions.
 * 
 * @par Function Calls: 
 * read_in_data(const char filename[], double bmi_data[][MAX], int max)\n
 * calculate_BMI(double bmi_data[][MAX], int bmi_group[], int num)\n
 * calculate_mean(double data[], int num)\n
 * calculate_stddev(double data[], double mean, int num)\n
 * write_histogram(const char filename[], int bmi_group[], int num)\n
 * write_stats(const char filename[], char data_name[], double mean, double stddev)\n
 *
 * @param[in] int argc
 * @param[in] char *argv[]
 *
 * @returns -1: No input entered, will display usage info
 *
 * @returns -2: Couldn't open the input file.
 *
 * @returns -3: Input file is empty
 *
 * @returns -4: Failed to open output file
 *
 * @returns -5: Failed to append output file
 *
 ******************************************************************************/
int main( int argc , char *argv[] )
{
   //Arrays
   double bmi_data[3][MAX] = {0}; //Stores height, weight and bmi data.
   int bmi_group[4] = {0}; //The bins defined by give ranges
   char dataname[3][7] = { "Height" , "Weight" , "BMI" };
   char filename[256] = "datagram.txt";
   double mean[3] = {0}; //Holds means for H, W, and BMI
   double stddev[3] = {0}; //Holds stddevs for H, W, BMI
   int num; //Number of lines read from infile

   //if we got no additional arguments, show usage info
   if (argc < 2)
   {
      cout << "Proper usage ... prog4.exe (in.txt) [out.txt]\n"
           <<	"                 (input filename), REQUIRED\n"
           << "                 [output filename], optional\n";
      return -1;
   }

   num = read_in_data( argv[1], bmi_data , MAX );
   if (num < 0) //bad file when return is negative
   {
      cout << "Couldn't open file: " << filename << endl;
      return -2;
   }
   else if( num == 0 ) //empty file when return is zero
   {
      cout << "File " << filename << " is empty.\n";
      return -3;
   }
	
   if( argc >= 3 ) //if we have a third argument, becomes output filename
      strcpy( filename, argv[2]);

   //Calculate the bmi 
   calculate_BMI( bmi_data, bmi_group , num );

   //Fills array of means and std_devs
   for ( int name = Height ; name <= BMI ; name++)
   {
      mean[name] = calculate_mean( bmi_data[name] , num );
      stddev[name] = calculate_stddev( bmi_data[name] , mean[name] , num );
   }

   //Call write_histogram and end program if it failed
   if( !write_histogram(filename, bmi_group , num) )
   {
      cout << "Error opening output file: " << filename << endl;
	  return -4;
   }

   //Append addition stats
   for ( int name = Height ; name <= BMI ; name++)
   {
      //If it failed to appened, display error and exit
      if( !write_stats(filename, dataname[name] , mean[name] , stddev[name] ) )
      {
         cout << "Error appending output file: " << filename << endl;
         return -5;
      }
   }//end for

   return 0;
}
/***************************************************************************//**
 * @author Noah Brubaker
 *
 * @par Description:
 * This function reads the data from an input file. It takes four parameters, 
 * two input (the name of the input file and max number of data elements) and
 * two output (bmi_data arrays). Before trying to read the data the 
 * function checks if the file was opened successfully. If successful, it will
 * return an integer telling how many elements were read from the input file. 
 * However, if it fails, the function will return a negative value.
 *
 * @param[in] const char filename[]
 * @param[in] int max
 * @param[out] double bmi_data[][MAX]
 *
 * @returns -1: Couldn't open the input file.
 * @returns  0: File empty
 * @returns  i: The number of complete pairs read from the file
 *
 ******************************************************************************/
int	read_in_data(const char filename[], double bmi_data[][MAX], int max)
{
   int i = 0; //line number
   ifstream fin; 
   fin.open(filename); //open file

   if( !fin )//if it failed return -1
      return -1;

   //read in first element of data line i
   while (fin >> bmi_data[Height][i] && i < max)
   {
      //read in second element of line i
      if (fin >> bmi_data[Weight][i])
         i++;
   }
   fin.close();//close file

   return i; //return number of data pairings
}
/***************************************************************************//**
 * @author Noah Brubaker
 *
 * @par Description:
 * This function has two input parameters (bmi_data and num) and two
 * output parameters (bmi_data[BMI] and bmi_group). It first calculates the BMI for each 
 * height/weight pair. It then assigns each BMI element to the correct bin 
 * (Underweight, Normal, Overweight or Obese) using the correct intervals, 
 * keeping count of the number elements assigned to each bin.
 *
 * @param[in] double bmi_data[][MAX]
 * @param[in] int num
 * @param[out] int bmi_group[]
 * @param[out] double bmi_data[BMI][MAX]
 * 
 * @returns none
 *
 ******************************************************************************/

void calculate_BMI(double bmi_data[][MAX], int bmi_group[], int num)
{
   double bmi = 0;
   double heightsqrd = 0;

   for ( int i = 0 ; i < num ; i++ )
   {
      //calculate height squared
      heightsqrd = (pow(bmi_data[Height][i], 2)); 
      //calculate bmi using given equation
      bmi = 703*bmi_data[Weight][i]/heightsqrd; 
      //fill data array with bmi's
      bmi_data[BMI][i] = bmi;
      //sort each bmi into bin as it is processed
      if ( bmi < 18.5 )
         bmi_group[Underweight]++;
      else if (bmi < 25)
         bmi_group[Normal]++;
      else if (bmi < 30)
         bmi_group[Overweight]++;
      else
         bmi_group[Obese]++;
   }//end for
}
/***************************************************************************//**
 * @author Noah Brubaker
 *
 * @par Description:
 * The purpose of the function is to calculate the mean of a set of data. It
 * takes two parameters: a data array, and the size of that array.
 *
 * @param[in] double data[]
 * @param[in] int num
 *
 * @returns sum/num = mean
 *
 ******************************************************************************/

double calculate_mean(double data[], int num)
{
   double sum = 0;
   for (int i = 0 ; i < num ; i++)
      sum += data[i]; //sums data
   return sum/num; //returns mean
}
/***************************************************************************//**
 * @author Noah Brubaker
 *
 * @par Description:
 * The purpose of the function is to calculate the standard deviation of a 
 * set of data. It takes three parameters: a data array, the size of that array
 * and the mean of the data.
 *
 * @param[in] double data[]
 * @param[in] double mean
 * @param[in] int num
 *
 * @returns stddev
 *
 ******************************************************************************/

double calculate_stddev(double data[], double mean, int num)
{
   double stddev = 0;
   /*stddev is the squareroot of the average of the square 
   of the difference between each data element and the mean 
   of the data*/
   for (int i = 0 ; i < num ; i++)
      stddev += pow( mean - data[i] , 2 ); //square difference
   stddev /= num; //average
   stddev = sqrt(stddev); //standard deviation

   return stddev;
}
/***************************************************************************//**
 * @author Noah Brubaker
 *
 * @par Description:
 * This function first opens an output file that is either defined by the user 
 * or defaulted to "datagram.txt". Before doing anything it checks to make sure
 * the file was successfully opened, if not returning false. If successful, it
 * calls the write_header function that gives the chart a title and defines n. 
 * It then writes the histogram of the data from bmi_group to the file. This is 
 * done by displaying horizontal bars made of stars that represent a certain
 * number of people. We are limited to 40 stars so we must base the size of the
 * star on the maximum group. We also need ticmarks that mark the location of
 * each 100 in the data. Even spacing is crucial for a clean looking chart.
 * If everything was successful, it returns true.
 *
 * @par Function Calls:
 * find_max( int data[] , int size )\n
 * write_header(ofstream & fout, int num)\n
 *
 * @param[in] const char filename[]
 * @param[in] int bmi_group[]
 * @param[in] int num
 *
 * @returns true: if writing was successful
 * @returns false: if unable to open output file
 *
 ******************************************************************************/

bool write_histogram(const char filename[], int bmi_group[], int num)
{
   int count = 0; //Used for making tic marks
   int max = 0; 
   double star = 1; //Value of each star
   double tics = 1; //Number of stars in a tic mark

   ofstream fout;
   fout.open(filename); //open file
   if ( !fout ) //check if successful
      return false;

      write_header( fout , num );
      max = find_max(bmi_group , 4); //max of the bmi_group
      star = max/40.0; //sets a value for the star
      tics = 100.0/star; //distance between tics
      fout << "---------------------------------------------------------\n";

      //Underweight
      fout << setw(17) << "|" << endl<< left << setw(12) << "Underweight:" 
           << right << setw(4) << bmi_group[Underweight] << "|";
      for (int i = 0 ; i < int(bmi_group[Underweight]/star + .5) ; i++ )
          fout << "*";
      fout << endl;

      //Normal
      fout << setw(17) << "|" << endl << left << setw(12) << "Normal:" 
           << right << setw(4) << bmi_group[Normal] << "|";
      for (int i = 0 ; i < int(bmi_group[Normal]/star + .5) ; i++ )
          fout << "*";
      fout << endl;

      //Overweight
      fout << setw(17) << "|" << endl << left << setw(12) << "Overweight:" 
           << right << setw(4) << bmi_group[Overweight] << "|";
      for (int i = 0 ; i < int(bmi_group[Overweight]/star + .5) ; i++ )
          fout << "*";
      fout << endl;

      //Obese heading
      fout << setw(17) << "|" << endl << left << setw(12) << "Obese:" 
           << right << setw(4) << bmi_group[Obese] << "|";
      for (int i = 0 ; i < int(bmi_group[Obese]/star + .5) ; i++ )
          fout << "*";
      fout << endl;

      //Tic marks
      fout << setw(17) << "|" << endl << "----------------";
      for ( int i = 0 ; i <= 40 ; i++ )
      {
          if ( i == int(tics+.5)*count )
          {
             fout << '+';
             count++;
          }
          else
             fout << '-';
      }
		
      //Number headings
      fout << endl << right << setw(17) << 0 << ' ';
      for (int i = 1 ; i <= 40/tics ; i++)
          fout << setw(int(tics + .5)) << 100*i;
      fout << endl;

   fout.close(); //close file
   return true;
}
/***************************************************************************//**
 * @author Noah Brubaker
 *
 * @par Description:
 * The purpose of this function is to demonstrate the passing of fstream
 * variables to a function. Has two input parameters and returns nothing.
 * Writes the header of the output file.
 *
 * @param[in] ofstream & fout
 * @param[in] int num
 *
 * @returns none
 *
 ******************************************************************************/

void write_header(ofstream & fout, int num)
{
   fout << "---------------------------------------------------------\n"
        << "Histogram showing BMI and Statistics\n"
        << "for Height and Weight n = " << num << endl;
}
/***************************************************************************//**
 * @author Noah Brubaker
 *
 * @par Description:
 * This function demonstrates the opening of files in append mode. It opens
 * the output file in this mode and checks if it was succesful, if not
 * returning false. If successful, it will display the Mean and StdDev of
 * the particular datum we've indicated in the call, and return true.
 *
 * @param[in] const char filename[]
 * @param[in] char data_name[]
 * @param[in] double mean
 * @param[in] double stddev
 *
 * @returns true: if appeneding was successful
 * @returns false: if it failed to open file
 *
 ******************************************************************************/

bool write_stats(const char filename[], char data_name[], double mean, double stddev)
{
   ofstream fout;
   fout.open(filename, ios::app); //open file in append mode
   if ( !fout ) //check if successful
      return false;

   fout << setprecision(2) << fixed << showpoint //set output precision
        << endl << left << setw(6) << data_name << ": "
        << "Mean:" << right << setw(7) << mean //print mean
        << " StdDev:" << setw(6) << stddev << endl; //print stddev

   fout.close(); //close file
   return true;
}
/***************************************************************************//**
 * @author Noah Brubaker
 *
 * @par Description:
 * The sole purpose of this function is to find the maximum value in an array
 * of integars.
 *
 * @param[in] int data[]
 * @param[in] int size
 *
 * @returns max
 *
 ******************************************************************************/

int find_max( int data[] , int size )
{
   int max = 0;
   for( int i = 0; i < size ; i++ )
      if(data[i] > max) //if element bigger than biggest so far...
      max = data[i];

   return max;
}


