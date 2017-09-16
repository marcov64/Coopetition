#include "fun_head.h"

MODELBEGIN

// f(x,y,a,b,c)=1/(1+abs(x**b-c-a*y**b))       
/*
You may want to change the fitness function to the "drop in the water" function, which is:

sinc(u,v) = sin(sqrt(u**2+v**2)) / sqrt(u**2+v**2)

(thanks to gnuplot demos)
*/

//DEBUG_AT(0);

EQUATION("ComputeFit")
/*
Function computing the fitness for an agent
*/
CYCLES(c,cur1, "Block")
 {
 CYCLES(cur1, cur, "ADim")
  {
  
  v[0]=VS(cur,"IdADim");
  //cur2=SEARCH_CND("IdDim",v[0]);
  cur2=cur->hook;
  v[1]=VS(cur,"x");
  WRITES(cur2,"X",v[1]);
  }
 }
v[2]=V("FunFitness");

RESULT(v[2] )

EQUATION("Fit")
/*
Mutate at least one dimension in the group

*/

v[9]=VS(p->up->up->up, "FairMutation");
v[10]=V("CounterMutation");
if(v[9]==1 && v[10]>0)
 {
 INCR("CounterMutation",-1);
 END_EQUATION(CURRENT);
 }


v[0]=VS_CHEAT(p->up->up->up,"ComputeFit", p);

cur=RNDDRAWFAIR("Block");
v[1]=VS(cur,"NDimBlock");
v[2]=rnd_integer(1,v[1]);
WRITE("CounterMutation",v[2]-1);
CYCLES(cur, cur1, "ADim")
  WRITES(cur1,"adflag",1);

v[4]=VS(p->up->up->up,"D");

for(v[3]=0; v[3]<v[2]; v[3]++)
 {
  cur1=RNDDRAWS(cur,"ADim","adflag");
  v[5]=VS(cur1,"x");
  v[6]=RND<0.5?-1:1;
  WRITES(cur1,"xtemp",v[5]);
  v[7]=v[5]+v[4]*v[6];
  WRITES(cur1,"x",v[7]);
  WRITES(cur1,"adflag",0);
 }
v[8]=VS_CHEAT(p->up->up->up,"ComputeFit", p);
WRITE("DiffMutFit",v[8]-v[0]);
if( v[8]<v[0])
 {
 CYCLES(cur, cur1, "ADim")
  {
   v[9]=VS(cur1,"adflag");
   if(v[9]==0)
    {
     v[10]=VS(cur1,"xtemp");
     WRITES(cur1,"x",v[10]);
    }
  }
  v[11]=v[0];
 }
else
 {
  v[11]=v[8];
  INCR("SuccessMutation",1);
//  INCR("MutatedBit",v[2]);
 }

RESULT(v[11] )

EQUATION("Fitness")
/*
Comment
*/
v[0]=v[1]=0;
CYCLE(cur, "Dim")
 {
  v[3]=VS(cur,"FContr");
  v[0]+=v[3];
  WRITES(cur,"FC_rec",v[3]);
  v[1]++;

 }


RESULT(v[0]/v[1] )

EQUATION("FunFitness")
/*
Comment
*/
v[0]=v[1]=0;
CYCLE(cur, "Dim")
 {
  v[0]+=VS(cur,"FContr");
  v[1]++;

 }


RESULT(v[0]/v[1] )

EQUATION("FContr")
/*
Comment

*/

v[0]=V("X");
v[1]=V("mu");
v[2]=V("beta");
v[3]=1/(1+abs(v[0]-v[1]));
RESULT( v[3])

EQUATION("mu")
/*
Comment
*/

v[0]=V("c");
if(V("numLink")>0)
{
CYCLE(cur, "Link")
 {
  if(cur->hook==NULL)
   {
    cur->hook=SEARCH_CNDS(p->up,"IdDim",VS(cur, "IdLink"));
   }
  
  v[0]+=VS(cur->hook,"X")*VS(cur, "a");

 }
}
RESULT(v[0] )


EQUATION("X")
/*
Comment
*/
v[2]=UNIFORM(98,102);
v[1]=1/V("D");
v[3]=round(v[2]*v[1]);
v[4]=v[3]/v[1];
/*
v[0]=V("IdDim");
if(v[0]==1)
 v[2]=UNIFORM(97.5,98.5);
else
 v[2]=UNIFORM(99.5,100.5);
*/

RESULT(v[4] )
//RESULT(UNIFORM(98,99) )

EQUATION("IsPeak")
/*
Comment
*/
v[0]=V("D");
v[1]=V("Fitness");
v[2]=0;
CYCLE(cur, "Dim")
 {
  INCRS(cur,"X",v[0]);
  v[3]=V("FunFitness");
  if(v[3]>v[1])
   v[2]=1;
  INCRS(cur,"X",-2*v[0]);
  v[3]=V("FunFitness");
  if(v[3]>v[1])
   v[2]=1;   
  INCRS(cur,"X",v[0]);  
 }
 
v[4]=v[2]==1?0:1;
RESULT(v[4] )

EQUATION("Steepest2")
/*
Compute the local maximum that can be reached from the current point, using, at each step, the steepest dimension.

To fix the problem at the first version I will limit the possibility to switch direction only once
*/
v[2]=0;
v[0]=V("D");
v[1]=V("Fitness");

CYCLE(cur, "Dim")
 {
  v[5]=VS(cur,"X");
  WRITES(cur,"Xc",v[5]);
  WRITES(cur,"app",1);
 }
 
v[10]=-1;
v[20]=-1;
while(v[2]==0)
 {
  v[10]++; //counter
  //until a local peak is found
  v[2]=1; //assume it is a peak

 CYCLE(cur, "Dim") //for any dimensiion
  {
  v[6]=0; //assume no improvement
  if(VS(cur, "app")!=0)
   {
  INCRS(cur,"X",v[0]); //try right
  v[3]=V("FunFitness");
  if(v[3]>v[1]) //improved
   {
    v[1]=v[3];//record new fitness
    v[6]=1; //record right
   } 
  INCRS(cur,"X",-2*v[0]); //try left
  v[3]=V("FunFitness");
  if(v[3]>v[1])
   {
    v[1]=v[3];   //record new fitness
    v[6]=-1; //record left
   } 
  INCRS(cur,"X",v[0]);  //restore original X
  if(v[6]!=0)
   { //improvement found
    v[7]=VS(cur,"IdDim"); //record dimesnion
    v[8]=v[6];//record direction
    v[2]=0; //keep on searching
   } 
  }
  }
  if(v[2]==0)
   {//improvement found
   
    cur=SEARCH_CND("IdDim",v[7]);
    v[9]=v[0]*v[8];
    INCRS(cur,"X",v[9]);
    if(v[20]==-1)
     v[20]=v[7];
    if(v[20]!=v[7])
     {
      cur1=SEARCH_CND("IdDim",v[20]);
      WRITES(cur1,"app",0);
     }  

   }
 }
CYCLE(cur, "Dim")
 {
  v[5]=VS(cur,"Xc");
  WRITELS(cur,"X",v[5],t);
 }

WRITE("StepSteepest",v[10]);
if(v[1]>0)
 V("RecordPath");

RESULT(v[1] )

EQUATION("RandomWalk")
/*
Move choosing randomly to a fitness-increasing dimension
*/

v[2]=0;
v[0]=V("D");
v[30]=v[0]/10;
//v[20]=V("Controlla");
v[20]=0;
if(v[20]==1)
 v[20]=INTERACT("Init",v[20]);

v[51]=v[1]=V("FunFitness");



CYCLE(cur, "Dim")
 {
  v[5]=VS(cur,"X");
  WRITES(cur,"Xc",v[5]);
 }
 
v[10]=-1;
while(v[2]==0)
 {
  v[10]++; //counter
  //until a local peak is found
  v[2]=1; //assume it is a peak
   CYCLE(cur, "Dim") //for any dimensiion
    WRITES(cur,"app",0);
if(v[20]==1)
 v[20]=INTERACT("Continue",v[20]);
 v[6]=0; //assume no improvement
 CYCLE(cur, "Dim") //for any dimensiion
  {

  v[31]=INCRS(cur,"X",v[0]); //try right
  v[3]=V("FunFitness");
  if(v[3]>v[1]) //improved
   {

      v[6]++; 
      WRITES(cur,"app",1);
      WRITES(cur,"beta",1);
   } 
   
  v[31]=INCRS(cur,"X",-2*v[0]); //try left
  v[3]=V("FunFitness");
  if(v[3]>v[1])
   {
    v[6]++; 
    WRITES(cur,"app",1);
    WRITES(cur,"beta",-1);

   } 
//  WRITES(cur,"MinusFit",v[3]);
  INCRS(cur,"X",v[0]);  //restore original X
  
  }
 if(v[10]==1)
  WRITE("NumDir",v[6]);

  if(v[6]>0)
   {//improvement found
   if(v[20]==1)
    INTERACT("Improvement found",v[6]);

    cur=RNDDRAW("Dim","app");
    
    v[8]=VS(cur,"beta");
    v[9]=v[0]*v[8];
   if(v[20]==1)
    INTERACTS(cur,"Chosen dim",v[9]);
    INCRS(cur,"X",v[9]);
    v[1]=V("FunFitness");
    v[2]=0;
   }
   else
   {
    if(v[20]==1 || v[20]==2 )
    INTERACT("Improvement NOT found",v[1]);

   }
 }
CYCLE(cur, "Dim")
 {
  WRITES(cur,"Xfin",VS(cur,"X"));
  v[5]=VS(cur,"Xc");
  WRITELS(cur,"X",v[5],t);
 }

WRITE("StepSteepest",v[10]);
if(v[1]>0)
 V("RecordPath");


RESULT(v[1] )

EQUATION("Steepest")
/*
Compute the local maximum that can be reached from the current point, using, at each step, the steepest dimension.

It is not satisfactory, since it melts down on the fringes: sometimes it can "ride" them up to the top, and other times it fails immediately. This is not good.
*/

v[2]=0;
v[0]=V("D");
v[20]=V("Controlla");
v[30]=0;

v[51]=v[1]=V("FunFitness");

CYCLE(cur, "Dim")
 {
  v[5]=VS(cur,"X");
  WRITES(cur,"Xc",v[5]);
 }
 
v[10]=-1;
while(v[2]==0)
 {
  v[10]++; //counter
  //until a local peak is found
  v[2]=1; //assume it is a peak
  
 CYCLE(cur, "Dim") //for any dimensiion
  {
  v[6]=0; //assume no improvement
  INCRS(cur,"X",v[0]); //try right
  v[3]=V("FunFitness");
  if(v[3]>v[1]+v[30]) //improved
   {
    v[1]=v[3];//record new fitness
    v[6]=1; //record right
   } 
  INCRS(cur,"X",-2*v[0]); //try left
  v[3]=V("FunFitness");
  if(v[3]>v[1]+v[30])
   {
    v[1]=v[3];   //record new fitness
    v[6]=-1; //record left
   } 
  INCRS(cur,"X",v[0]);  //restore original X
  if(v[6]!=0)
   { //improvement found
    v[7]=VS(cur,"IdDim"); //record dimesnion
    v[8]=v[6];//record direction
    v[2]=0; //keep on searching
   } 
  }
  if(v[20]==2)
   {
    plog("\n");
    CYCLE(cur, "Dim")
    {
     v[21]=VS(cur,"X");
     sprintf(msg, "%g\t",v[21]);
     plog(msg);
    }
    sprintf(msg, "%g",v[1]);
    plog(msg);
    
   }  
  if(v[2]==0)
   {//improvement found
   
    cur=SEARCH_CND("IdDim",v[7]);
    v[9]=v[0]*v[8];
    INCRS(cur,"X",v[9]);
    v[30]=(v[1]-v[51])*0;
    v[51]=v[1];

   }
 }
if(v[20]==2) 
 {
  sprintf(msg,"\nStepSteepes %g %g %g\n",v[10], v[51],v[1]);
  plog(msg);
 } 
 
CYCLE(cur, "Dim")
 {
  v[5]=VS(cur,"Xc");
  WRITELS(cur,"X",v[5],t);
 }

WRITE("StepSteepest",v[10]);
if(v[1]>0)
 V("RecordPath");

RESULT(v[1] )


EQUATION("Steepest3")
/*
Compute the local maximum that can be reached from the current point, using, at each step, the steepest dimension.

It is not satisfactory, since it melts down on the fringes: sometimes it can "ride" them up to the top, and other times it fails immediately. This is not good.

Attempts to fix the zig-zag problem on the edges by testing D for each step, but making the actual steps of D/2

*/

v[2]=0;
v[0]=V("D");
v[21]=v[1]=V("Fitness");

CYCLE(cur, "Dim")
 {
  v[5]=VS(cur,"X");
  WRITES(cur,"Xc",v[5]);
 }
 
v[10]=-1;
while(v[2]==0)
 {
  v[10]++; //counter
  //until a local peak is found
  v[2]=1; //assume it is a peak
  
 CYCLE(cur, "Dim") //for any dimensiion
  {
  v[6]=0; //assume no improvement
  INCRS(cur,"X",v[0]); //try right
  v[3]=V("FunFitness");
  if(v[3]>v[1]) //improved
   {
    v[1]=v[3];//record new fitness
    v[6]=1; //record right
   } 
  INCRS(cur,"X",-2*v[0]); //try left
  v[3]=V("FunFitness");
  if(v[3]>v[1])
   {
    v[1]=v[3];   //record new fitness
    v[6]=-1; //record left
   } 
  INCRS(cur,"X",v[0]);  //restore original X
  if(v[6]!=0)
   { //improvement found
    v[7]=VS(cur,"IdDim"); //record dimesnion
    v[8]=v[6];//record direction
    v[2]=0; //keep on searching
   } 
  }
  
  if(v[2]==0)
   {//improvement found
    cur=SEARCH_CND("IdDim",v[7]);
    v[9]=v[0]*v[8]/2;
    INCRS(cur,"X",v[9]);
    v[11]=V("FunFitness");
    if(v[11]<v[21])
     {v[2]=1;
      INCRS(cur,"X",-1*v[9]);
      v[1]=V("FunFitness");
     } 
    else
     v[21]=v[1]=v[11]; 
   }
 }
CYCLE(cur, "Dim")
 {
  v[5]=VS(cur,"Xc");
  WRITELS(cur,"X",v[5],t);
 }

WRITE("StepSteepest",v[10]);
if(v[1]>0)
 V("RecordPath");

RESULT(v[1] )


EQUATION("c")
/*
Constant
*/
v[0]=0;
if(V("numLink")>0)
{
CYCLE(cur, "Link")
 {
  v[0]+=VS(cur,"a");
 }
}


v[2]=V("XOptimal");
v[0]*=v[2];
v[1]=v[2]-v[0];

RESULT(v[1] )

EQUATION("ShiftFrontier")
/*
Comment
*/
cur=RNDDRAWFAIR("Dim");
v[0]=V("ShiftOpt");
INCRS(cur,"XOptimal",v[0]);


RESULT(1 )


EQUATION("XOptimal")
/*
Optimal value for the dimension
*/

v[0]=VL("XOptimal",1);
v[1]=V("ShiftOpt");
v[2]=-v[1];

v[3]=v[0]+UNIFORM(v[1],v[2]);
RESULT(v[3] )


EQUATION("RecordPath")
/*
Comment
*/

v[2]=0;
v[0]=V("D");
v[1]=V("Fitness");

plog("\\nBegins\\n");
CYCLE(cur, "Dim")
 {
  v[5]=VS(cur,"X");
  WRITES(cur,"Xc",v[5]);
  sprintf(msg, " %g", v[5]);
  plog(msg);
 }
 
plog("\\n");
v[10]=-1;
while(v[2]==0)
 {
  v[10]++; //counter
  //until a local peak is found
  v[2]=1; //assume it is a peak
 CYCLE(cur, "Dim") //for any dimensiion
  {
   v[6]=0; //assume no improvement
  INCRS(cur,"X",v[0]); //try right
  v[3]=V("FunFitness");
  if(v[3]>v[1]) //improved
   {
    v[1]=v[3];//record new fitness
    v[6]=1; //record right
   } 
  INCRS(cur,"X",-2*v[0]); //try left
  v[3]=V("FunFitness");
  if(v[3]>v[1])
   {
    v[1]=v[3];   //record new fitness
    v[6]=-1; //record left
   } 
  INCRS(cur,"X",v[0]);  //restore original X
  if(v[6]!=0)
   { //improvement found
    v[7]=VS(cur,"IdDim"); //record dimesnion
    v[8]=v[6];//record direction
    v[2]=0; //keep on searching
   } 
  }
  
  if(v[2]==0)
   {//improvement found
    sprintf(msg, "%d(%d)", (int)v[7], (int)v[8]);
    plog(msg);
    cur=SEARCH_CND("IdDim",v[7]);
    v[9]=v[0]*v[8];
    v[13]=INCRS(cur,"X",v[9]);
    CYCLE(cur, "Dim")
     {
      v[5]=VS(cur,"X");
      sprintf(msg, " %g", v[5]);
      plog(msg);
     }
     plog("\\n");
   }
 }
CYCLE(cur, "Dim")
 {
  v[5]=VS(cur,"Xc");
  WRITELS(cur,"X",v[5],t);
 }

if(V("StepSteepest")!=v[10])
 INTERACT("cazzo", v[10]);
RESULT(v[1] )


EQUATION("InitA")
/*
Initialize the a's with signs of a_i,j =+ if i<j and =- if i>j 
*/
v[0]=V("aij");
v[6]=V("Group");
v[7]=0;
v[4]=1;
v[10]=V("N");
cur1=SEARCH("Dim");
cur=SEARCHS(cur1,"Link");
v[21]=1/V("D");

ADDNOBJ_EX("Dim",v[10]-1, cur1);

v[11]=V("Minx");
v[12]=V("Maxx");

CYCLE(cur, "Dim")
 {v[1]=v[4]++;
  if( ((int)(v[1]-1)%(int)v[6]) ==0)
    v[7]++;
  WRITES(cur,"IdDim",v[1]);
  v[13]=UNIFORM(v[11],v[12]);
  
  v[23]=round(v[13]*v[21]);
  v[24]=v[23]/v[21];

  WRITES(cur,"X",v[24]);
  v[5]=1;
  ADDNOBJS(cur,"Link",v[10]-2);
  CYCLES(cur, cur1, "Link")
   {
    if(v[5]==v[1])
     v[5]++;
    if(v[5]> (v[7]-1)*v[6] && v[5]<= v[7]*v[6])
      v[8]=1;
    else
      v[8]=0;  
    WRITES(cur1,"IdLink",v[5]++); 
    if(v[5]>v[1])
     v[3]=1;
    else
     v[3]=-1;
    WRITES(cur1,"a",v[0]*v[3]*v[8]);  
   }
 }
 
// INTERACT("BEFORE DELETE", v[6]);
CYCLE(cur, "Dim")
 {
 v[15]=0;
 CYCLE_SAFES(cur, cur1, "Link")
   {v[15]++;
    v[14]=VS(cur1,"a");
    if(v[14]==0)
     {
      DELETE(cur1);
      v[15]--;
     } 
   }
  WRITES(cur,"numLink",v[15]); 
 }
//INTERACT("AFTER DELETE", v[6])

PARAMETER
RESULT(1 )

EQUATION("Init")
/*
Initialize all worlds
*/
CYCLE(cur, "World")
 {
  VS(cur,"InitA");
 }
PARAMETER
RESULT( 1)


EQUATION("InitAgent")
/*
Initialize agents
*/
v[0]=V("N");
//v[1]=V("NBlock");
//v[2]=ceil(v[0]/v[1])-1;
v[2]=V("AGroup")-1;
v[1]=ceil(v[0]/(v[2]+1));
v[5]=V("Minx");
v[6]=V("Maxx");

v[12]=V("AllEqual");
v[21]=V("D");
CYCLE(cur, "Agent")
 {
  v[3]=v[4]=1;
//  cur1=SEARCHS(cur,"Block");
//  cur3=SEARCHS(cur,"ADim");

  ADDNOBJS(cur,"Block",v[1]-1);
  CYCLES(cur, cur2, "Block")
    {
     WRITES(cur2,"IdBlock",v[3]++);
     ADDNOBJS(cur2,"ADim",v[2]);
     CYCLES(cur2, cur4, "ADim")
       {
        cur5=SEARCH_CND("IdDim",v[4]);
        cur4->hook=cur5;
        WRITES(cur4,"IdADim",v[4]++);
        if(v[12]!=1)
          {
           v[13]=UNIFORM(v[5],v[6]);
           v[23]=round(v[13]/v[21]);
           v[24]=v[23]*v[21];
           v[7]=v[24];
          } 
        else
         {
         cur5=SEARCH_CND("IdDim",v[4]-1);
         v[7]=VS(cur5,"X");
         } 
         WRITES(cur4,"x",v[7]);

       } 
     WRITES(cur2,"NDimBlock",v[2]+1);  
    }
   v[8]=V_CHEAT("ComputeFit",cur);
   WRITELS(cur,"Fit",v[8], t);
 }
PARAMETER;
RESULT( 1)


EQUATION("AvFit")
/*
Average fitness
*/
v[0]=v[1]=0;
v[2]=v[3]=0;
v[4]=v[5]=0;

CYCLE(cur, "Agent")
 {
  v[0]+=VS(cur,"Fit");
  v[1]++;
  v[2]+=VS(cur,"SuccessMutation");
  v[4]+=VS(cur,"MutatedBit");
 }

WRITE("AvSucMut",v[2]/v[1]);
WRITE("AvMutBit",v[4]/v[1]);
RESULT(v[0]/v[1] )

MODELEND





void close_sim(void)
{

}

/*

Tommaso trento

0461-261671
st. 112-103
*/

