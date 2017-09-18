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
  if(VS(cur2,"InConsortium")==0)
    v[1]=VS(cur,"x");
  else
    v[1]=VS(cur2->hook,"cX");  
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

cur=NULL;
while(cur==NULL)
{
cur=RNDDRAWFAIR("Block");
v[30]=0;
CYCLES(cur, cur1, "ADim")
 {
  if(VS(cur1->hook,"InConsortium")==0)
    v[30]++;
 }
 if(v[30]==0)
  cur=NULL; 
}
//v[1]=VS(cur,"NDimBlock");
v[1]=v[30];
v[2]=rnd_integer(1,v[1]);
WRITE("CounterMutation",v[2]-1);
CYCLES(cur, cur1, "ADim")
  {
   if(VS(cur1->hook,"InConsortium")==0)
     WRITES(cur1,"adflag",1);
   else
     WRITES(cur1,"adflag",0);     
  } 

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
  VS(cur,"CInit");
 }
PARAMETER
RESULT( 1)

EQUATION("ConsortiumRD")
/*
Research in the Consortium
*/

v[1]=V("numDimConsortium");
if(v[1]==0)
 END_EQUATION(0);
CYCLE(cur, "CDim")
 {
  v[0]=VS(cur,"cX");
  WRITES(cur->hook,"X",v[0]);
  WRITES(cur,"cXtemp",v[0]);
  WRITES(cur,"capp",1);
 }

v[12]=V("CriterionConsortium");
v[13]=V("ShareAccept");
v[14]=V("NumTrials");
v[22]=0;
v[4]=V("D");
cur3=SEARCHS(p->up,"Class");

for(v[15]=0; v[15]<v[14]; v[15]++)
{
  cur=RNDDRAW("CDim","capp");
  v[5]=RND<0.5?v[4]:v[4]*-1;
  INCRS(cur,"cX",v[5]);

  v[0]=v[1]=v[20]=v[21]=0;
  CYCLES(cur3, cur1, "Agent")
   {v[21]++;
    v[10]=VS(cur1,"Fit");
    v[20]+=v[10];
    v[11]=V_CHEAT("ComputeFit",cur1);
    if(v[10]<v[11])
     v[0]++;
    v[1]+=v[11];
   }
  if(v[22]==0)
   {
    v[23]=v[0];
    v[24]=v[1];
   }  

  if(v[12]==1)
   {//Majority rule, mutation accepted if the majority gains
    v[13]=V("ShareAccept");
    if(v[0]>v[21]*v[13] && v[0]>=v[23])
     {
      v[23]=v[0];
      v[22]=1;
      CYCLE(cur, "CDim")
       {
        v[30]=VS(cur,"cX");
        WRITES(cur,"cXbest",v[30]);
       }
     }  
   }
  if(v[12]==2)
   {//Average rule, mutation accepted if the sum of fitness increases
    
    if(v[1]>v[20]*v[13] && v[1]>=v[24])
     {
      v[22]=1;
      v[24]=v[1];
      CYCLE(cur, "CDim")
       {
        v[30]=VS(cur,"cX");
        WRITES(cur,"cXbest",v[30]);
       }
     } 
   }
  }
  
  if(v[22]==0)
   {
    CYCLE(cur, "CDim")
     {
      v[30]=VS(cur,"cXtemp");
      WRITES(cur,"cX",v[30]);
     }
      
   } 
  else
   {
    CYCLE(cur, "CDim")
     {
      v[30]=VS(cur,"cXbest");
      WRITES(cur,"cX",v[30]);
     }
   CYCLES(cur3, cur1, "Agent")
    {
      v[11]=V_CHEAT("ComputeFit",cur1);
      WRITELS(cur1,"Fit",v[11], t);
    }  

    INCR("SuccCMutation",1);
   }
 
  
 
RESULT(v[22] )


EQUATION("CInit")
/*
Initialization of the consortium
*/

v[1]=V("N");
v[0]=V("numDimConsortium");
if(v[0]==0)
 END_EQUATION(0);
ADDNOBJ("CDim",v[1]-1);

v[3]=1;
v[5]=V("Minx");
v[6]=V("Maxx");
v[21]=V("D");

CYCLE(cur, "CDim")
 {
  cur1=SEARCH_CND("IdDim",v[3]);
  cur->hook=cur1;
  cur1->hook=cur;
  WRITES(cur1,"InConsortium",1);  
  WRITES(cur,"IdCDim",v[3]++);
  v[13]=UNIFORM(v[5],v[6]);
  v[23]=round(v[13]/v[21]);
  v[24]=v[23]*v[21];
  v[7]=v[24];
  WRITES(cur,"cX",v[7]);

 }

for(v[2]=0; v[2]<v[1]-v[0]; v[2]++)
 {
  cur=RNDDRAWFAIR("CDim");
  cur->hook->hook=NULL;
  WRITES(cur->hook,"InConsortium",0);
  DELETE(cur);
 }
PARAMETER;
RESULT(1 )


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

