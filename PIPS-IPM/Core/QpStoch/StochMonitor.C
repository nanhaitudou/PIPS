#include "StochMonitor.h"
#include "Status.h"

#include "Residuals.h"
#include "Solver.h"
#include "QpGenData.h"
#include "QpGenVars.h"
#include "sTree.h"
#include <iostream>
#include <cstdio>

using namespace std;

StochMonitor::StochMonitor(QpGenStoch* qp_) 
{
  qp = qp_;
  mpiComm=MPI_COMM_WORLD; //default for old version
  MPI_Comm_rank(mpiComm, &myRank);
  myGlobRank = myRank;
}

StochMonitor::StochMonitor(sFactory* qp_) 
{
  mpiComm = qp_->tree->commWrkrs;
  MPI_Comm_rank(mpiComm, &myRank);
  MPI_Comm_rank(MPI_COMM_WORLD, &myGlobRank);
}
void StochMonitor::doIt( Solver * solver, Data * data, Variables * vars,
			 Residuals * resids,
			 double alpha, double sigma,
			 int i, double mu,
			 int status_code,
			 int level ) 
{
  double objective = dynamic_cast<QpGenData*>(data)->objectiveValue(dynamic_cast<QpGenVars*>(vars));

  //log only on the first proc
  if(myRank>0) return;

  double dnorm = solver->dataNorm();

  switch( level ) {
  case 0 : case 1: { 
    cout << " --- Iteration " << i << " --- (rank " << myGlobRank << ")" << endl;
    if(i==1)
      printf(" mu = %16.12e  rel.res.norm=%16.12e  datanorm=%16.12e\n", 
	     mu, resids->residualNorm() / dnorm, dnorm);
    else
      printf(" mu = %16.12e  rel.res.norm=%16.12e\n",
             mu, resids->residualNorm() / dnorm);
    //cout << " mu = " << mu << " relative residual norm = " 
    //cout << resids->residualNorm() / dnorm << endl;
    cout << " Duality Gap:  " << resids->dualityGap() << endl;
    if( i > 1 ) {
      cout << " alpha = " << alpha << endl;
    }
    cout << " Objective: " << objective << endl;
    cout << endl;
    if( level == 1) { 
      // Termination has been detected by the status check; print
      // appropriate message
      switch( status_code ) {
      case SUCCESSFUL_TERMINATION:
	cout << endl << " *** SUCCESSFUL TERMINATION ***" << endl;
	break;
      case MAX_ITS_EXCEEDED:
	cout << endl << " *** MAXIMUM ITERATIONS REACHED *** " << endl;
	break;
      case INFEASIBLE:
	cout << endl << " *** TERMINATION: PROBABLY INFEASIBLE *** " << endl;
      case UNKNOWN:
	cout << endl << " *** TERMINATION: STATUS UNKNOWN *** " << endl;
	break;
      } // end switch(statusCode)
    }
  } break; // end case 0: case 1:
  } // end switch(level)
}
