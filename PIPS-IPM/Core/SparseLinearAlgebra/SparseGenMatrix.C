/* OOQP                                                               *
 * Authors: E. Michael Gertz, Stephen J. Wright                       *
 * (C) 2001 University of Chicago. See Copyright Notification in OOQP */

#include "SparseGenMatrix.h"
#include "SparseStorage.h"
#include <cassert>
#include "SimpleVector.h"

#include "DoubleMatrixTypes.h"

#include "SparseSymMatrix.h"

int SparseGenMatrix::isKindOf( int type )
{
  return type == kSparseGenMatrix || type == kGenMatrix;
}


SparseGenMatrix::SparseGenMatrix( int rows, int cols, int nnz )
  : m_Mt(NULL)
{
  mStorage = SparseStorageHandle( new SparseStorage( rows, cols, nnz ) );
}


SparseGenMatrix::SparseGenMatrix( int rows, int cols, int nnz,
				  int krowM[], int jcolM[],  double M[],
				  int deleteElts)
  : m_Mt(NULL)
{
  //cout << "SparseGenMatrix1  " << rows << " " << cols << " " << nnz << endl;
  mStorage = SparseStorageHandle( new SparseStorage( rows, cols,nnz, 
						     krowM, jcolM, M,
						     deleteElts) );
}

/*SparseGenMatrix::SparseGenMatrix(const std::vector<SparseGenMatrix*> &blocks, bool diagonal)
  : m_Mt(NULL)
{
  vector<SparseStorage*> v(blocks.size());
  for (size_t i = 0; i < blocks.size(); i++) v[i] = blocks[i]->mStorage;
  mStorage = SparseStorageHandle(new SparseStorage(v,diagonal));
}
*/

SparseGenMatrix::~SparseGenMatrix()
{
  //cout << "~~~~~~~~~SparseGenMatrix " << mStorage->mRefs  << endl;
  if(m_Mt) delete m_Mt;
  
}


void SparseGenMatrix::atPutDense( int row, int col, double * A, int lda,
				      int rowExtent, int colExtent )
{
  mStorage->atPutDense( row, col, A, lda, rowExtent, colExtent );
}


void SparseGenMatrix::fromGetDense( int row, int col, double * A, int lda,
					int rowExtent, int colExtent )
{
  mStorage->fromGetDense( row, col, A, lda, rowExtent, colExtent );
}
  

void SparseGenMatrix::fromGetSpRow( int row, int col,
				    double A[], int lenA,
				    int jcolA[], int& nnz,
				    int colExtent, int& info )
{
  mStorage->fromGetSpRow( row, col, A, lenA, jcolA, nnz,
			  colExtent, info );
}


void SparseGenMatrix::putSparseTriple( int irow[], int len,
					   int jcol[], double A[], 
					   int& info )
{
  mStorage->putSparseTriple( irow, len, jcol, A, info );
}


void SparseGenMatrix::writeToStream(ostream& out) const
{
  mStorage->writeToStream( out );
}


void SparseGenMatrix::randomize( double alpha, double beta, double * seed )
{
  mStorage->randomize( alpha, beta, seed );
}


void SparseGenMatrix::getDiagonal( OoqpVector& vec )
{
  mStorage->getDiagonal( vec );
}


void SparseGenMatrix::setToDiagonal( OoqpVector& vec )
{
  mStorage->setToDiagonal( vec );
}


void SparseGenMatrix::atPutSpRow( int row, double A[],
				      int lenA, int jcolA[], int& info )
{
  mStorage->atPutSpRow( row, A, lenA, jcolA, info );
}


int SparseGenMatrix::numberOfNonZeros()
{
  return mStorage->numberOfNonZeros();
}


void SparseGenMatrix::symmetrize( int& info ) 
{
  mStorage->symmetrize( info );
}


void SparseGenMatrix::getSize( long long& m, long long& n )
{
  m = mStorage->m;
  n = mStorage->n;
}
void SparseGenMatrix::getSize( int& m, int& n )
{
  m = mStorage->m;
  n = mStorage->n;
}


void SparseGenMatrix::atPutSubmatrix( int destRow, int destCol,
					  DoubleMatrix& M,
					  int srcRow, int srcCol,
					  int rowExtent, int colExtent )
{
  int i, k;
  int info, nnz;

  int *    ja = new int[colExtent];
  double * a  = new double[colExtent];

  nnz = 0;
  for ( i = 0; i < rowExtent; i++ ) {
    M.fromGetSpRow( srcRow + i, srcCol, a, colExtent, ja,
		     nnz, colExtent, info );
    for( k = 0; k < nnz; k++ ) {
      ja[k] += (destCol - srcCol);
    }
    mStorage->atPutSpRow( destRow + i, a, nnz, ja, info );
  }

  delete [] ja;
  delete [] a;
}


void SparseGenMatrix::mult ( double beta,  OoqpVector& y_in,
				 double alpha, OoqpVector& x_in )
{
  SimpleVector & x = dynamic_cast<SimpleVector &>(x_in);
  SimpleVector & y = dynamic_cast<SimpleVector &>(y_in);
  
  assert( x.n == mStorage->n && y.n == mStorage->m );

  double *xv = 0, *yv = 0;

  if( x.n > 0 ) xv = &x[0];
  if( y.n > 0 ) yv = &y[0];

  mStorage->mult( beta, yv, 1, alpha, xv, 1 );
}

void SparseGenMatrix::mult ( double beta,  double y[], int incy,
			     double alpha, double x[], int incx )
{
  mStorage->mult( beta, y, incy, alpha, x, incx);
}


void SparseGenMatrix::transMult ( double beta,   OoqpVector& y_in,
				  double alpha,  OoqpVector& x_in )
{
  SimpleVector & x = dynamic_cast<SimpleVector &>(x_in);
  SimpleVector & y = dynamic_cast<SimpleVector &>(y_in);

  assert( x.n == mStorage->m && y.n == mStorage->n );

  double *xv = 0, *yv = 0;

  if( x.n > 0 ) xv = &x[0];
  if( y.n > 0 ) yv = &y[0];

  mStorage->transMult( beta, yv, 1, alpha, xv, 1 );
}

// wrapper added by cpetra
void SparseGenMatrix::transMult( double beta,  OoqpVector& y_in, int incy,
			         double alpha, OoqpVector& x_in, int incx )
{
  SimpleVector & x = dynamic_cast<SimpleVector &>(x_in);
  SimpleVector & y = dynamic_cast<SimpleVector &>(y_in);
  
  assert(x.n>0 && y.n>0);
  assert(x.n>=incx*mStorage->m);
  assert(y.n>=incy*mStorage->n);

  double *xv = 0, *yv = 0;

  if( x.n > 0 ) xv = &x[0];
  if( y.n > 0 ) yv = &y[0];

  mStorage->transMult( beta, yv, incy, alpha, xv, incx );
}

// wrapper added by cpetra
void SparseGenMatrix::transMult( double beta,  double yv[], int incy,
				 double alpha, double xv[], int incx )
{
  mStorage->transMult( beta, yv, incy, alpha, xv, incx );
}


double SparseGenMatrix::abmaxnorm()
{
  return mStorage->abmaxnorm();
}


void SparseGenMatrix::atPutDiagonal( int idiag, OoqpVector& vvec )
{
  SimpleVector & v = dynamic_cast<SimpleVector &>(vvec);

  mStorage->atPutDiagonal( idiag, &v[0], 1, v.n );
}


void SparseGenMatrix::fromGetDiagonal( int idiag, OoqpVector& vvec )
{
  mStorage->fromGetDiagonal( idiag, vvec );
}

void SparseGenMatrix::ColumnScale( OoqpVector& vec )
{
  mStorage->ColumnScale( vec );
}

void SparseGenMatrix::SymmetricScale( OoqpVector& vec )
{
  mStorage->SymmetricScale( vec );
}

void SparseGenMatrix::RowScale( OoqpVector& vec )
{
  mStorage->ColumnScale( vec );
}

void SparseGenMatrix::scalarMult( double num )
{
  mStorage->scalarMult( num );
}

void SparseGenMatrix::matTransDMultMat(OoqpVector& d_, SymMatrix** res)
{
  SimpleVector& d = dynamic_cast<SimpleVector &>(d_);

  int m=mStorage->m; int n=mStorage->n; int nnz=mStorage->numberOfNonZeros();

  if(*res==NULL) {
    assert(m_Mt==NULL);
    //we need to form the transpose
    m_Mt=new SparseGenMatrix(n,m,nnz);
    mStorage->transpose(m_Mt->krowM(), m_Mt->jcolM(), m_Mt->M());

    //find the sparsity pattern of the product -> the buffers for result will be allocated
    int* krowMtM=NULL; int* jcolMtM=NULL; double* dMtM=NULL;
    mStorage->matTransDSymbMultMat(&d[0], 
				   m_Mt->krowM(), m_Mt->jcolM(), m_Mt->M(),
				   &krowMtM, &jcolMtM, &dMtM);

    *res = new SparseSymMatrix(n, krowMtM[n], krowMtM, jcolMtM, dMtM, 1);
  }

  assert(res); 
  assert(m_Mt);

  SparseSymMatrix* MtDM = dynamic_cast<SparseSymMatrix*>(*res);

  mStorage->matTransDMultMat(&d[0], 
			     m_Mt->krowM(), m_Mt->jcolM(), m_Mt->M(),
			     MtDM->krowM(), MtDM->jcolM(), MtDM->M());
}

void SparseGenMatrix::matTransDinvMultMat(OoqpVector& d_, SymMatrix** res)
{
  SimpleVector& d = dynamic_cast<SimpleVector &>(d_);

  int m=mStorage->m; int n=mStorage->n; int nnz=mStorage->numberOfNonZeros();

  if(*res==NULL) {
    assert(m_Mt==NULL);
    //we need to form the transpose
    m_Mt=new SparseGenMatrix(n,m,nnz);
    mStorage->transpose(m_Mt->krowM(), m_Mt->jcolM(), m_Mt->M());

    //find the sparsity pattern of the product -> the buffers for result will be allocated
    int* krowMtM=NULL; int* jcolMtM=NULL; double* dMtM=NULL;
    mStorage->matTransDSymbMultMat(&d[0], 
				   m_Mt->krowM(), m_Mt->jcolM(), m_Mt->M(),
				   &krowMtM, &jcolMtM, &dMtM);

    *res = new SparseSymMatrix(n, krowMtM[n], krowMtM, jcolMtM, dMtM, 1);
  }

  assert(res); 
  assert(m_Mt);

  SparseSymMatrix* MtDM = dynamic_cast<SparseSymMatrix*>(*res);

  mStorage->matTransDinvMultMat(&d[0], 
			     m_Mt->krowM(), m_Mt->jcolM(), m_Mt->M(),
			     MtDM->krowM(), MtDM->jcolM(), MtDM->M());
}
void SparseGenMatrix::matMultTrans(SymMatrix** res)
{
  int m=mStorage->m; int n=mStorage->n; 
  int nnz=mStorage->numberOfNonZeros();

  SimpleVector d(n); d.setToConstant(1.0);

  if(*res==NULL) {
    //assert(m_Mt==NULL);

    //we need to form the transpose
    if(!m_Mt) {
      m_Mt = new SparseGenMatrix(n,m,nnz);
      mStorage->transpose(m_Mt->krowM(), m_Mt->jcolM(), m_Mt->M());
    }
    //find the sparsity pattern of the product -> the buffers for result will be allocated
    int* krowMtM=NULL; int* jcolMtM=NULL; double* dMtM=NULL;

    m_Mt->mStorage->matTransDSymbMultMat(&d[0], 
					 krowM(), jcolM(), M(),
					 &krowMtM, &jcolMtM, &dMtM);
    *res = new SparseSymMatrix(m, krowMtM[m], krowMtM, jcolMtM, dMtM, 1);
  }
  SparseSymMatrix* MMt = dynamic_cast<SparseSymMatrix*>(*res);
  m_Mt->mStorage->matTransDMultMat(&d[0],
				   krowM(), jcolM(), M(),
				   MMt->krowM(), MMt->jcolM(), MMt->M());
}
