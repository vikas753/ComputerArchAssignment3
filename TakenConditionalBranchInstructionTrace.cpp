#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<iterator>
#include<cstdio>

using namespace std;

typedef signed long int INT128;

// Data structure to hold Instruction address 
// Whether that particular Instruction address is 
// a taken branch conditional or Loop conditional
typedef struct
{
  int TakenBranchConditionalFlag;
  int NonTakenBranchConditionFlag;
  int LoopConditional;
  INT128 IAddressTrace;
} addressTrace_t;

// Database for containing Instruction Address trace
vector<addressTrace_t> IAddressTrace;

#undef TRUE
#undef FALSE
#define TRUE  1
#define FALSE 0

// Check whether a given Instruction address is a conditional branch or not
// based on it's current delta with consecutive address with that of previous 
// address to see if there is a change if so ,then it is a conditional
// branch for sure

#define CONSECUTIVE_LOCATION_DIFFERENCE 2
#define CONSECUTIVE_LOCATION_DIFFERENCE_3 3

int isTakenBranchConditional(addressTrace_t*TraceVar , INT128 NextIAddress)
{
  INT128 AddressDifference = NextIAddress - TraceVar->IAddressTrace ; 
  for(int i=0;i<IAddressTrace.size();i++)
  {
    if(TraceVar->IAddressTrace == IAddressTrace[i].IAddressTrace)
    {
      INT128 PrevAddressDelta = IAddressTrace[i+1].IAddressTrace - IAddressTrace[i].IAddressTrace;
      if(AddressDifference != PrevAddressDelta)
      {
         
        if((PrevAddressDelta == CONSECUTIVE_LOCATION_DIFFERENCE) || (PrevAddressDelta == CONSECUTIVE_LOCATION_DIFFERENCE_3))
        {
          TraceVar->NonTakenBranchConditionFlag = TRUE;
          IAddressTrace[i].TakenBranchConditionalFlag = TRUE;
        }
        
        if((AddressDifference == CONSECUTIVE_LOCATION_DIFFERENCE)||(AddressDifference == CONSECUTIVE_LOCATION_DIFFERENCE_3))
        {
          return TRUE;  
        } 
      } 
    }
  }
  return FALSE;
}

// Check whether a given INstruction address is a loop by doing a 
// difference between and current and next address to see
// if there is a negative displacement . 


int isLoopConditional(addressTrace_t TraceVar , INT128 NextIAddress)
{
  INT128 AddressDifference = NextIAddress - TraceVar.IAddressTrace ;
  return (AddressDifference < 0)?TRUE:FALSE; 
}

void DisplayNumberofConditionalBranches()
{
  int NumTakenCondBranches = 0 , NumNonTakenCondBranches = 0;
  for(int i=0;i<IAddressTrace.size();i++)
  {
    if((IAddressTrace[i].TakenBranchConditionalFlag == TRUE) || (IAddressTrace[i].LoopConditional == TRUE))
    {
      NumTakenCondBranches++;
    }
    else if(IAddressTrace[i].NonTakenBranchConditionFlag == TRUE)
    {
      NumNonTakenCondBranches++;
    }
  }
  cout << " Number of Taken conditional branches is : " << NumTakenCondBranches << endl; 
  cout << " Number of Non Taken conditional branches is : " << NumNonTakenCondBranches << endl;
  cout << " Number of Conditional Branches is : " << ( NumTakenCondBranches + NumNonTakenCondBranches ) << endl;

}

// Below Api , Simulates 1 bit branch predictor

// State Machine for one bit branch predictor simulator it
// should be either FALSE or TRUE
int BranchStateOneBit = FALSE;

void BranchPredictorOneBit()
{
  int NumMispredictions = 0,NumCorrectPredictions = 0;
  for(int i=0;i<IAddressTrace.size();i++)
  {
    if((IAddressTrace[i].TakenBranchConditionalFlag == TRUE) || (IAddressTrace[i].LoopConditional == TRUE))
    {
      BranchStateOneBit = (BranchStateOneBit == FALSE)?TRUE:FALSE; 
      NumMispredictions = (BranchStateOneBit == FALSE)?(NumMispredictions+1):NumMispredictions;    
      NumCorrectPredictions = (BranchStateOneBit == FALSE)?(NumCorrectPredictions):(NumCorrectPredictions+1);     
    }
    else if(IAddressTrace[i].NonTakenBranchConditionFlag == TRUE)
    {
      BranchStateOneBit = (BranchStateOneBit == FALSE)?FALSE:TRUE; 
      NumMispredictions = (BranchStateOneBit == FALSE)?(NumMispredictions):(NumMispredictions+1);    
      NumCorrectPredictions = (BranchStateOneBit == FALSE)?(NumCorrectPredictions+1):(NumCorrectPredictions);      
    }
  }
  cout << "1-bit branch predictor -  Number of Mispredictions : " << NumMispredictions << " NumCorrectPredictions : " << NumCorrectPredictions << endl;
}

// Below APi , Simulates a 2-bit branch predictor

// Below vector or array stores 16 counters ( 2-bit )

#define NUM_TWO_BIT_COUNTERS 16

// Below enums depict the state of 2-bit counter
#define TWO_BIT_COUNTER_NT_DEFINED              0
#define TWO_BIT_COUNTER_WEAKLY_NT_DEFINED       1
#define TWO_BIT_COUNTER_WEAKLY_T_DEFINED        2
#define TWO_BIT_COUNTER_T_DEFINED               3

vector<int> TwoBitPredictorVector;

void BranchPredictTwoBit()
{
  TwoBitPredictorVector.assign(NUM_TWO_BIT_COUNTERS,TWO_BIT_COUNTER_WEAKLY_NT_DEFINED);
  int NumMispredictions = 0 , NumCorrectPredictions = 0;
  for(int i=0;i<IAddressTrace.size();i++)
  {
    int BranchCounterIndex = IAddressTrace[i].IAddressTrace % NUM_TWO_BIT_COUNTERS ; 
    if((IAddressTrace[i].TakenBranchConditionalFlag == TRUE) || (IAddressTrace[i].LoopConditional == TRUE))
    {
      NumMispredictions = (TwoBitPredictorVector[BranchCounterIndex] <= TWO_BIT_COUNTER_WEAKLY_NT_DEFINED)?(NumMispredictions+1):NumMispredictions;    
      NumCorrectPredictions =(TwoBitPredictorVector[BranchCounterIndex] > TWO_BIT_COUNTER_WEAKLY_NT_DEFINED)?(NumCorrectPredictions+1):(NumCorrectPredictions);     
      TwoBitPredictorVector[BranchCounterIndex] = ( TwoBitPredictorVector[BranchCounterIndex] <  TWO_BIT_COUNTER_T_DEFINED)?TwoBitPredictorVector[BranchCounterIndex] + 1 : TwoBitPredictorVector[BranchCounterIndex]; 
    }
    else if(IAddressTrace[i].NonTakenBranchConditionFlag == TRUE)
    {
      NumMispredictions = (TwoBitPredictorVector[BranchCounterIndex] > TWO_BIT_COUNTER_WEAKLY_NT_DEFINED)?(NumMispredictions+1):NumMispredictions;    
      NumCorrectPredictions =(TwoBitPredictorVector[BranchCounterIndex] <= TWO_BIT_COUNTER_WEAKLY_NT_DEFINED)?(NumCorrectPredictions+1):(NumCorrectPredictions);     
      TwoBitPredictorVector[BranchCounterIndex] = ( TwoBitPredictorVector[BranchCounterIndex] >  TWO_BIT_COUNTER_NT_DEFINED)?TwoBitPredictorVector[BranchCounterIndex] - 1 : TwoBitPredictorVector[BranchCounterIndex]; 
 
    }
    
  }
   cout << " 2-bit branch predictor - Number of Mispredictions : " << NumMispredictions << " NumCorrectPredictions : " << NumCorrectPredictions << endl; 
}

typedef struct
{
  int NumTakenBranches;
  int NumNonTakenBranches;
  INT128 PageAddress;
} pageStruct_t;


int initialState = TRUE ; 
vector<pageStruct_t> PageAndBranchesTable;

// Inserts a Page onto Table and ensures to sort it
void InsertPageOntoTable(pageStruct_t PageVar)
{
  PageAndBranchesTable.push_back(PageVar);
  int IterIndex = PageAndBranchesTable.size()-1;
  while((IterIndex > 0) && (PageAndBranchesTable[IterIndex].PageAddress < PageAndBranchesTable[IterIndex-1].PageAddress))
  {
    pageStruct_t PageTemp = PageAndBranchesTable[IterIndex];
    PageAndBranchesTable[IterIndex] = PageAndBranchesTable[IterIndex-1];
    PageAndBranchesTable[IterIndex-1] = PageTemp;  
  }
}

#define PAGE_BOUNDARY 2048


#define WEIGHT_PAGE_ONE   10
#define WEIGHT_PAGE_TWO   6
#define WEIGHT_PAGE_THREE 3

int calculateOutputPage(int index)
{
  int output =  WEIGHT_PAGE_ONE*(PageAndBranchesTable[index].NumTakenBranches - PageAndBranchesTable[index].NumNonTakenBranches);
  int next_page_index = index+1;
  int prev_page_index = index-1; 
  if(next_page_index < PageAndBranchesTable.size())
  {
    output = output + WEIGHT_PAGE_TWO*(PageAndBranchesTable[next_page_index].NumTakenBranches - PageAndBranchesTable[next_page_index].NumNonTakenBranches);
  }
  if(prev_page_index >= 0)
  {
    output = output + WEIGHT_PAGE_THREE*(PageAndBranchesTable[prev_page_index].NumTakenBranches - PageAndBranchesTable[prev_page_index].NumNonTakenBranches);  
  }
  return output;
}

// Find Page Address in Page and Branches Table
int FindIndexInPageAndBranchesTable(INT128 PageAddressTemp)
{
  for(int i=0;i<PageAndBranchesTable.size();i++)
  {
    if(PageAndBranchesTable[i].PageAddress == PageAddressTemp)
    {
      return i;
    }
  }
  return -1;
}

#define THRESHOLD 200

// Alternative Algorithm for predicting branches
// using paging as one of weights . 


void BranchPredictorSpecialAlgo()
{
  int NumCorrectPredictions = 0 ,  NumMispredictions = 0;
  int THRESHOLD_VAR  = THRESHOLD;
  for(int i=0;i<IAddressTrace.size();i++)
  {
    if((IAddressTrace[i].TakenBranchConditionalFlag == TRUE) || (IAddressTrace[i].LoopConditional == TRUE) || (IAddressTrace[i].NonTakenBranchConditionFlag  == TRUE))
    {
      INT128 PageAddressTemp = ( IAddressTrace[i].IAddressTrace / 2048 ) * 2048;
      pageStruct_t PageStruct;
      int index = FindIndexInPageAndBranchesTable(PageAddressTemp);   
      PageStruct.PageAddress = PageAddressTemp;
      if((IAddressTrace[i].TakenBranchConditionalFlag == TRUE) || (IAddressTrace[i].LoopConditional == TRUE))
      {
        PageStruct.NumTakenBranches = 1;
      }
      else
      {
        PageStruct.NumNonTakenBranches = 1;
      }
      // Find if this Page exists or not in Vector Container . 
      if(index == -1)
      {
        if(PageStruct.NumTakenBranches == 1)
        {
          NumCorrectPredictions =  NumCorrectPredictions + 1;
        }
        else
        {
          NumMispredictions = NumMispredictions + 1;
        }
        InsertPageOntoTable(PageStruct);
      }
      else
      {
        if(calculateOutputPage(index)>THRESHOLD_VAR)
        {
          // Predicted to be taken
          if(PageStruct.NumTakenBranches == 1)
          {
            NumCorrectPredictions++;
            PageAndBranchesTable[index].NumTakenBranches++;  
          }
          else
          {
            NumMispredictions++;
            THRESHOLD_VAR= THRESHOLD_VAR+10; 
            PageAndBranchesTable[index].NumNonTakenBranches++;      
          }
        }
        else
        {
           // Predicted to be non-taken
          if(PageStruct.NumNonTakenBranches == 1)
          {
            NumCorrectPredictions++;
            PageAndBranchesTable[index].NumNonTakenBranches++;  
          }
          else
          {
            NumMispredictions++;
            PageAndBranchesTable[index].NumTakenBranches++;      
            THRESHOLD_VAR=THRESHOLD_VAR-10;
          }       
        }      
      }
    } 
  }  
   cout << " Special ALgorithm branch predictor - Number of Mispredictions : " << NumMispredictions << " NumCorrectPredictions : " << NumCorrectPredictions << endl; 
}

int main()
{
  // Open a file in read mode , initialize iterators
  ifstream input("itrace.out");
  istream_iterator<string> input_iterator(input);

  istream_iterator<string> eoi;
  INT128 currentAddress = stoll(*input_iterator , 0 , 16);
  INT128 nextAddress = 0xFFFFFFFFFFFFFFFF;
  
  int imax = 100000;
 
  // Iteration over a file
  while(imax>0)
  {
    addressTrace_t TraceTempVar;   
    input_iterator++;
    if((input_iterator)!=eoi)
    {
      nextAddress = stoll(*input_iterator,0,16);
    }
 
    TraceTempVar.IAddressTrace = currentAddress;
    TraceTempVar.TakenBranchConditionalFlag = isTakenBranchConditional(&TraceTempVar,nextAddress);
    TraceTempVar.LoopConditional = isLoopConditional(TraceTempVar,nextAddress);

   // printf("IATrace : 0x%x , TBCF : %d , LC : %d \n" , TraceTempVar.IAddressTrace , TraceTempVar.TakenBranchConditionalFlag , TraceTempVar.LoopConditional);

    IAddressTrace.push_back(TraceTempVar); 
    currentAddress = nextAddress;
    imax--;
  }

  DisplayNumberofConditionalBranches();
  BranchPredictorOneBit(); 
  BranchPredictTwoBit();
  BranchPredictorSpecialAlgo();
  return 0;
}
