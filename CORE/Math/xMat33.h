#ifndef FOUNDATION_xMat33T
#define FOUNDATION_xMat33T
/*----------------------------------------------------------------------------*\
|
|						Public Interface to Ageia PhysX Technology
|
|							     www.ageia.com
|
\*----------------------------------------------------------------------------*/
/** \addtogroup foundation
  @{
*/

#include "xVec3.h"
#include "xQuat.h"


/**
\brief Identifies a special matrix. Can be passed to the #xMat33 constructor.
*/
enum xMatrixType
	{
	/**
	\brief Matrix of all zeros.
	*/
	X_ZERO_MATRIX,

	/**
	\brief Identity matrix.
	*/
	X_IDENTITY_MATRIX
	};

#include "x9F32.h"				//change this if changing type below, to x9F32, x12F32, x16F32		


/**
\brief 3x3 Matrix Class.

 The idea of the matrix/vector classes is to partition them into two parts:
 One is the data structure which may have different formatting (3x3, 3x4, 4x4),
 row or column major.  The other is a template class which has all the operators
 but is storage format independent.

 This way it should be easier to change formats depending on what is faster/slower
 on a particular platform.

 Design issue: We use nameless struct/unions here.
 Design issue: this used to be implemented with a template.  This had no benefit
 but it added syntactic complexity.  Currently we just use a typedef and a preprocessor switch 
 to change between different memory layouts.

 The matrix math in this class is storage format (row/col major) independent as far
 as the user is concerned.
 When the user wants to get/set raw data, he needs to specify what order the data is
 coming in.  
 
*/
class xMat33
	{
	public:
	X_INLINE xMat33();

	/**
	\param type Special matrix type to initialize with.

	@see xMatrixType
	*/
	X_INLINE xMat33(xMatrixType type);
	X_INLINE xMat33(const xVec3 &row0, const xVec3 &row1, const xVec3 &row2);

	X_INLINE xMat33(const xMat33&m);
	X_INLINE xMat33(const xQuat &m);
	X_INLINE ~xMat33();
	X_INLINE const xMat33& operator=(const xMat33 &src);

	// Access elements

	//low level data access, single or double precision, with eventual translation:
	//for dense 9 element data
	X_INLINE void setRowMajor(const xF32 *);
	X_INLINE void setRowMajor(const xF32 d[][3]);
	X_INLINE void setColumnMajor(const xF32 *);
	X_INLINE void setColumnMajor(const xF32 d[][3]);
	X_INLINE void getRowMajor(xF32 *) const;
	X_INLINE void getRowMajor(xF32 d[][3]) const;
	X_INLINE void getColumnMajor(xF32 *) const;
	X_INLINE void getColumnMajor(xF32 d[][3]) const;

	X_INLINE void setRowMajor(const xF64 *);
	X_INLINE void setRowMajor(const xF64 d[][3]);
	X_INLINE void setColumnMajor(const xF64 *);
	X_INLINE void setColumnMajor(const xF64 d[][3]);
	X_INLINE void getRowMajor(xF64 *) const;
	X_INLINE void getRowMajor(xF64 d[][3]) const;
	X_INLINE void getColumnMajor(xF64 *) const;
	X_INLINE void getColumnMajor(xF64 d[][3]) const;


	//for loose 4-padded data.
	X_INLINE void setRowMajorStride4(const xF32 *);
	X_INLINE void setRowMajorStride4(const xF32 d[][4]);
	X_INLINE void setColumnMajorStride4(const xF32 *);
	X_INLINE void setColumnMajorStride4(const xF32 d[][4]);
	X_INLINE void getRowMajorStride4(xF32 *) const;
	X_INLINE void getRowMajorStride4(xF32 d[][4]) const;
	X_INLINE void getColumnMajorStride4(xF32 *) const;
	X_INLINE void getColumnMajorStride4(xF32 d[][4]) const;

	X_INLINE void setRowMajorStride4(const xF64 *);
	X_INLINE void setRowMajorStride4(const xF64 d[][4]);
	X_INLINE void setColumnMajorStride4(const xF64 *);
	X_INLINE void setColumnMajorStride4(const xF64 d[][4]);
	X_INLINE void getRowMajorStride4(xF64 *) const;
	X_INLINE void getRowMajorStride4(xF64 d[][4]) const;
	X_INLINE void getColumnMajorStride4(xF64 *) const;
	X_INLINE void getColumnMajorStride4(xF64 d[][4]) const;


	X_INLINE void setRow(int row, const xVec3 &);
	X_INLINE void setColumn(int col, const xVec3 &);
	X_INLINE void getRow(int row, xVec3 &) const;
	X_INLINE void getColumn(int col, xVec3 &) const;

	X_INLINE xVec3 getRow(int row) const;
	X_INLINE xVec3 getColumn(int col) const;


	//element access:
    X_INLINE xReal & operator()(int row, int col);
    X_INLINE const xReal & operator() (int row, int col) const;

	/**
	\brief returns true for identity matrix
	*/
	X_INLINE bool isIdentity() const;

	/**
	\brief returns true for zero matrix
	*/
	X_INLINE bool isZero() const;

	/**
	\brief returns true if all elems are finite (not NAN or INF, etc.)
	*/
	X_INLINE bool isFinite() const;

	//create special matrices:

	/**
	\brief sets this matrix to the zero matrix.
	*/
	X_INLINE void zero();

	/**
	\brief sets this matrix to the identity matrix.
	*/
	X_INLINE void id();

	/**
	\brief this = -this
	*/
	X_INLINE void setNegative();

	/**
	\brief sets this matrix to the diagonal matrix.
	*/
	X_INLINE void diagonal(const xVec3 &vec);

	/**
	\brief Sets this matrix to the Star(Skew Symetric) matrix.

	So that star(v) * x = v.cross(x) .
	*/
	X_INLINE void star(const xVec3 &vec);


	X_INLINE void fromQuat(const xQuat &);
	X_INLINE void toQuat(xQuat &) const;

	//modifications:

	X_INLINE const xMat33 &operator +=(const xMat33 &s);
	X_INLINE const xMat33 &operator -=(const xMat33 &s);
	X_INLINE const xMat33 &operator *=(xReal s);
	X_INLINE const xMat33 &operator /=(xReal s);

	/*
	Gram-Schmidt orthogonalization to correct numerical drift, plus column normalization
	Caution: I believe the current implementation does not work right!
	*/
//	X_INLINE void orthonormalize();


	/**
	\brief returns determinant
	*/
	X_INLINE xReal determinant() const;

	/**
	\brief assigns inverse to dest.
	
	Returns false if singular (i.e. if no inverse exists), setting dest to identity.
	*/
	X_INLINE bool getInverse(xMat33& dest) const;

	/**
	\brief this = transpose(other)

	this == other is OK.
	*/
	X_INLINE void setTransposed(const xMat33& other);

	/**
	\brief this = transpose(this)
	*/
	X_INLINE void setTransposed();

	/**
	\brief this = this * [ d.x 0 0; 0 d.y 0; 0 0 d.z];
	*/
	X_INLINE void multiplyDiagonal(const xVec3 &d);

	/**
	\brief this = transpose(this) * [ d.x 0 0; 0 d.y 0; 0 0 d.z];
	*/
	X_INLINE void multiplyDiagonalTranspose(const xVec3 &d);

	/**
	\brief dst = this * [ d.x 0 0; 0 d.y 0; 0 0 d.z];
	*/
	X_INLINE void multiplyDiagonal(const xVec3 &d, xMat33 &dst) const;

	/**
	\brief dst = transpose(this) * [ d.x 0 0; 0 d.y 0; 0 0 d.z];
	*/
	X_INLINE void multiplyDiagonalTranspose(const xVec3 &d, xMat33 &dst) const;

	/**
	\brief dst = this * src
	*/
	X_INLINE void multiply(const xVec3 &src, xVec3 &dst) const;
	/**
	\brief dst = transpose(this) * src
	*/
	X_INLINE void multiplyByTranspose(const xVec3 &src, xVec3 &dst) const;

	/**
	\brief this = a + b
	*/
	X_INLINE void  add(const xMat33 & a, const xMat33 & b);
	/***
	\brief this = a - b
	*/
	X_INLINE void  subtract(const xMat33 &a, const xMat33 &b);
	/**
	\brief this = s * a;
	*/
	X_INLINE void  multiply(xReal s,  const xMat33 & a);
	/**
	\brief this = left * right
	*/
	X_INLINE void multiply(const xMat33& left, const xMat33& right);
	/**
	\brief this = transpose(left) * right

	\note #multiplyByTranspose() is faster.
	*/
	X_INLINE void multiplyTransposeLeft(const xMat33& left, const xMat33& right);
	/**
	\brief this = left * transpose(right)
	
	\note faster than #multiplyByTranspose().
	*/
	X_INLINE void multiplyTransposeRight(const xMat33& left, const xMat33& right);

	/**
	\brief this = left * transpose(right)
	*/
	X_INLINE void multiplyTransposeRight(const xVec3 &left, const xVec3 &right);

	/**
	\brief this = rotation matrix around X axis

	<b>Unit:</b> Radians
	*/
	X_INLINE void rotX(xReal angle);

	/**
	\brief this = rotation matrix around Y axis

	<b>Unit:</b> Radians
	*/
	X_INLINE void rotY(xReal angle);

	/**
	\brief this = rotation matrix around Z axis

	<b>Unit:</b> Radians
	*/
	X_INLINE void rotZ(xReal angle);


	//overloaded multiply, and transposed-multiply ops:

	/**
	\brief returns transpose(this)*src
	*/
	X_INLINE xVec3 operator%  (const xVec3 & src) const;
	/**
	\brief matrix vector product
	*/
	X_INLINE xVec3 operator*  (const xVec3 & src) const;
	/**
	\brief matrix product
	*/
	X_INLINE xMat33&	operator*= (const xMat33& mat);
	/**
	\brief matrix difference
	*/
	X_INLINE xMat33	operator-  (const xMat33& mat)	const;
	/**
	\brief matrix addition
	*/
	X_INLINE xMat33	operator+  (const xMat33& mat)	const;
	/**
	\brief matrix product
	*/
	X_INLINE xMat33	operator*  (const xMat33& mat)	const;
	/**
	\brief matrix scalar product
	*/
	X_INLINE xMat33	operator*  (float s)				const;

	private:
	x9Real data;
	};


X_INLINE xMat33::xMat33()
	{
	}


X_INLINE xMat33::xMat33(xMatrixType type)
	{
		switch(type)
		{
			case X_ZERO_MATRIX:		zero();	break;
			case X_IDENTITY_MATRIX:	id();	break;
		}
	}


X_INLINE xMat33::xMat33(const xMat33& a)
	{
	data = a.data;
	}


X_INLINE xMat33::xMat33(const xQuat &q)
	{
	fromQuat(q);
	}

X_INLINE xMat33::xMat33(const xVec3 &row0, const xVec3 &row1, const xVec3 &row2)
{
	data.s._11 = row0.x;  data.s._12 = row0.y;  data.s._13 = row0.z;
	data.s._21 = row1.x;  data.s._22 = row1.y;  data.s._23 = row1.z;
	data.s._31 = row2.x;  data.s._32 = row2.y;  data.s._33 = row2.z;
}


X_INLINE xMat33::~xMat33()
	{
	//nothing
	}


X_INLINE const xMat33& xMat33::operator=(const xMat33 &a)
	{
	data = a.data;
	return *this;
	}


X_INLINE void xMat33::setRowMajor(const xF32* d)
	{
	//we are also row major, so this is a direct copy
	data.s._11 = (xReal)d[0];
	data.s._12 = (xReal)d[1];
	data.s._13 = (xReal)d[2];

	data.s._21 = (xReal)d[3];
	data.s._22 = (xReal)d[4];
	data.s._23 = (xReal)d[5];

	data.s._31 = (xReal)d[6];
	data.s._32 = (xReal)d[7];
	data.s._33 = (xReal)d[8];
	}


X_INLINE void xMat33::setRowMajor(const xF32 d[][3])
	{
	//we are also row major, so this is a direct copy
	data.s._11 = (xReal)d[0][0];
	data.s._12 = (xReal)d[0][1];
	data.s._13 = (xReal)d[0][2];

	data.s._21 = (xReal)d[1][0];
	data.s._22 = (xReal)d[1][1];
	data.s._23 = (xReal)d[1][2];

	data.s._31 = (xReal)d[2][0];
	data.s._32 = (xReal)d[2][1];
	data.s._33 = (xReal)d[2][2];
	}


X_INLINE void xMat33::setColumnMajor(const xF32* d)
	{
	//we are column major, so copy transposed.
	data.s._11 = (xReal)d[0];
	data.s._12 = (xReal)d[3];
	data.s._13 = (xReal)d[6];

	data.s._21 = (xReal)d[1];
	data.s._22 = (xReal)d[4];
	data.s._23 = (xReal)d[7];

	data.s._31 = (xReal)d[2];
	data.s._32 = (xReal)d[5];
	data.s._33 = (xReal)d[8];
	}


X_INLINE void xMat33::setColumnMajor(const xF32 d[][3])
	{
	//we are column major, so copy transposed.
	data.s._11 = (xReal)d[0][0];
	data.s._12 = (xReal)d[1][0];
	data.s._13 = (xReal)d[2][0];

	data.s._21 = (xReal)d[0][1];
	data.s._22 = (xReal)d[1][1];
	data.s._23 = (xReal)d[2][1];

	data.s._31 = (xReal)d[0][2];
	data.s._32 = (xReal)d[1][2];
	data.s._33 = (xReal)d[2][2];
	}


X_INLINE void xMat33::getRowMajor(xF32* d) const
	{
	//we are also row major, so this is a direct copy
	d[0] = (xF32)data.s._11;
	d[1] = (xF32)data.s._12;
	d[2] = (xF32)data.s._13;

	d[3] = (xF32)data.s._21;
	d[4] = (xF32)data.s._22;
	d[5] = (xF32)data.s._23;

	d[6] = (xF32)data.s._31;
	d[7] = (xF32)data.s._32;
	d[8] = (xF32)data.s._33;
	}


X_INLINE void xMat33::getRowMajor(xF32 d[][3]) const
	{
	//we are also row major, so this is a direct copy
	d[0][0] = (xF32)data.s._11;
	d[0][1] = (xF32)data.s._12;
	d[0][2] = (xF32)data.s._13;

	d[1][0] = (xF32)data.s._21;
	d[1][1] = (xF32)data.s._22;
	d[1][2] = (xF32)data.s._23;

	d[2][0] = (xF32)data.s._31;
	d[2][1] = (xF32)data.s._32;
	d[2][2] = (xF32)data.s._33;
	}


X_INLINE void xMat33::getColumnMajor(xF32* d) const
	{
	//we are column major, so copy transposed.
	d[0] = (xF32)data.s._11;
	d[3] = (xF32)data.s._12;
	d[6] = (xF32)data.s._13;

	d[1] = (xF32)data.s._21;
	d[4] = (xF32)data.s._22;
	d[7] = (xF32)data.s._23;

	d[2] = (xF32)data.s._31;
	d[5] = (xF32)data.s._32;
	d[8] = (xF32)data.s._33;
	}


X_INLINE void xMat33::getColumnMajor(xF32 d[][3]) const
	{
	//we are column major, so copy transposed.
	d[0][0] = (xF32)data.s._11;
	d[1][0] = (xF32)data.s._12;
	d[2][0] = (xF32)data.s._13;

	d[0][1] = (xF32)data.s._21;
	d[1][1] = (xF32)data.s._22;
	d[2][1] = (xF32)data.s._23;

	d[0][2] = (xF32)data.s._31;
	d[1][2] = (xF32)data.s._32;
	d[2][2] = (xF32)data.s._33;
	}


X_INLINE void xMat33::setRowMajorStride4(const xF32* d)
	{
	//we are also row major, so this is a direct copy
	//however we've got to skip every 4th element.
	data.s._11 = (xReal)d[0];
	data.s._12 = (xReal)d[1];
	data.s._13 = (xReal)d[2];

	data.s._21 = (xReal)d[4];
	data.s._22 = (xReal)d[5];
	data.s._23 = (xReal)d[6];

	data.s._31 = (xReal)d[8];
	data.s._32 = (xReal)d[9];
	data.s._33 = (xReal)d[10];
	}


X_INLINE void xMat33::setRowMajorStride4(const xF32 d[][4])
	{
	//we are also row major, so this is a direct copy
	data.s._11 = (xReal)d[0][0];
	data.s._12 = (xReal)d[0][1];
	data.s._13 = (xReal)d[0][2];

	data.s._21 = (xReal)d[1][0];
	data.s._22 = (xReal)d[1][1];
	data.s._23 = (xReal)d[1][2];

	data.s._31 = (xReal)d[2][0];
	data.s._32 = (xReal)d[2][1];
	data.s._33 = (xReal)d[2][2];
	}


X_INLINE void xMat33::setColumnMajorStride4(const xF32* d)
	{
	//we are column major, so copy transposed.
	//however we've got to skip every 4th element.
	data.s._11 = (xReal)d[0];
	data.s._12 = (xReal)d[4];
	data.s._13 = (xReal)d[8];

	data.s._21 = (xReal)d[1];
	data.s._22 = (xReal)d[5];
	data.s._23 = (xReal)d[9];

	data.s._31 = (xReal)d[2];
	data.s._32 = (xReal)d[6];
	data.s._33 = (xReal)d[10];
	}


X_INLINE void xMat33::setColumnMajorStride4(const xF32 d[][4])
	{
	//we are column major, so copy transposed.
	data.s._11 = (xReal)d[0][0];
	data.s._12 = (xReal)d[1][0];
	data.s._13 = (xReal)d[2][0];

	data.s._21 = (xReal)d[0][1];
	data.s._22 = (xReal)d[1][1];
	data.s._23 = (xReal)d[2][1];

	data.s._31 = (xReal)d[0][2];
	data.s._32 = (xReal)d[1][2];
	data.s._33 = (xReal)d[2][2];
	}


X_INLINE void xMat33::getRowMajorStride4(xF32* d) const
	{
	//we are also row major, so this is a direct copy
	//however we've got to skip every 4th element.
	d[0] = (xF32)data.s._11;
	d[1] = (xF32)data.s._12;
	d[2] = (xF32)data.s._13;

	d[4] = (xF32)data.s._21;
	d[5] = (xF32)data.s._22;
	d[6] = (xF32)data.s._23;

	d[8] = (xF32)data.s._31;
	d[9] = (xF32)data.s._32;
	d[10]= (xF32)data.s._33;
	}


X_INLINE void xMat33::getRowMajorStride4(xF32 d[][4]) const
	{
	//we are also row major, so this is a direct copy
	d[0][0] = (xF32)data.s._11;
	d[0][1] = (xF32)data.s._12;
	d[0][2] = (xF32)data.s._13;

	d[1][0] = (xF32)data.s._21;
	d[1][1] = (xF32)data.s._22;
	d[1][2] = (xF32)data.s._23;

	d[2][0] = (xF32)data.s._31;
	d[2][1] = (xF32)data.s._32;
	d[2][2] = (xF32)data.s._33;
	}


X_INLINE void xMat33::getColumnMajorStride4(xF32* d) const
	{
	//we are column major, so copy transposed.
	//however we've got to skip every 4th element.
	d[0] = (xF32)data.s._11;
	d[4] = (xF32)data.s._12;
	d[8] = (xF32)data.s._13;

	d[1] = (xF32)data.s._21;
	d[5] = (xF32)data.s._22;
	d[9] = (xF32)data.s._23;

	d[2] = (xF32)data.s._31;
	d[6] = (xF32)data.s._32;
	d[10]= (xF32)data.s._33;
	}


X_INLINE void xMat33::getColumnMajorStride4(xF32 d[][4]) const
	{
	//we are column major, so copy transposed.
	d[0][0] = (xF32)data.s._11;
	d[1][0] = (xF32)data.s._12;
	d[2][0] = (xF32)data.s._13;

	d[0][1] = (xF32)data.s._21;
	d[1][1] = (xF32)data.s._22;
	d[2][1] = (xF32)data.s._23;

	d[0][2] = (xF32)data.s._31;
	d[1][2] = (xF32)data.s._32;
	d[2][2] = (xF32)data.s._33;
	}


X_INLINE void xMat33::setRowMajor(const xF64*d)
	{
	//we are also row major, so this is a direct copy
	data.s._11 = (xReal)d[0];
	data.s._12 = (xReal)d[1];
	data.s._13 = (xReal)d[2];

	data.s._21 = (xReal)d[3];
	data.s._22 = (xReal)d[4];
	data.s._23 = (xReal)d[5];

	data.s._31 = (xReal)d[6];
	data.s._32 = (xReal)d[7];
	data.s._33 = (xReal)d[8];
	}


X_INLINE void xMat33::setRowMajor(const xF64 d[][3])
	{
	//we are also row major, so this is a direct copy
	data.s._11 = (xReal)d[0][0];
	data.s._12 = (xReal)d[0][1];
	data.s._13 = (xReal)d[0][2];

	data.s._21 = (xReal)d[1][0];
	data.s._22 = (xReal)d[1][1];
	data.s._23 = (xReal)d[1][2];

	data.s._31 = (xReal)d[2][0];
	data.s._32 = (xReal)d[2][1];
	data.s._33 = (xReal)d[2][2];
	}


X_INLINE void xMat33::setColumnMajor(const xF64*d)
	{
	//we are column major, so copy transposed.
	data.s._11 = (xReal)d[0];
	data.s._12 = (xReal)d[3];
	data.s._13 = (xReal)d[6];

	data.s._21 = (xReal)d[1];
	data.s._22 = (xReal)d[4];
	data.s._23 = (xReal)d[7];

	data.s._31 = (xReal)d[2];
	data.s._32 = (xReal)d[5];
	data.s._33 = (xReal)d[8];
	}


X_INLINE void xMat33::setColumnMajor(const xF64 d[][3])
	{
	//we are column major, so copy transposed.
	data.s._11 = (xReal)d[0][0];
	data.s._12 = (xReal)d[1][0];
	data.s._13 = (xReal)d[2][0];

	data.s._21 = (xReal)d[0][1];
	data.s._22 = (xReal)d[1][1];
	data.s._23 = (xReal)d[2][1];

	data.s._31 = (xReal)d[0][2];
	data.s._32 = (xReal)d[1][2];
	data.s._33 = (xReal)d[2][2];
	}


X_INLINE void xMat33::getRowMajor(xF64*d) const
	{
	//we are also row major, so this is a direct copy
	d[0] = (xF64)data.s._11;
	d[1] = (xF64)data.s._12;
	d[2] = (xF64)data.s._13;

	d[3] = (xF64)data.s._21;
	d[4] = (xF64)data.s._22;
	d[5] = (xF64)data.s._23;

	d[6] = (xF64)data.s._31;
	d[7] = (xF64)data.s._32;
	d[8] = (xF64)data.s._33;
	}


X_INLINE void xMat33::getRowMajor(xF64 d[][3]) const
	{
	//we are also row major, so this is a direct copy
	d[0][0] = (xF64)data.s._11;
	d[0][1] = (xF64)data.s._12;
	d[0][2] = (xF64)data.s._13;

	d[1][0] = (xF64)data.s._21;
	d[1][1] = (xF64)data.s._22;
	d[1][2] = (xF64)data.s._23;

	d[2][0] = (xF64)data.s._31;
	d[2][1] = (xF64)data.s._32;
	d[2][2] = (xF64)data.s._33;
	}


X_INLINE void xMat33::getColumnMajor(xF64*d) const
	{
	//we are column major, so copy transposed.
	d[0] = (xF64)data.s._11;
	d[3] = (xF64)data.s._12;
	d[6] = (xF64)data.s._13;

	d[1] = (xF64)data.s._21;
	d[4] = (xF64)data.s._22;
	d[7] = (xF64)data.s._23;

	d[2] = (xF64)data.s._31;
	d[5] = (xF64)data.s._32;
	d[8] = (xF64)data.s._33;
	}


X_INLINE void xMat33::getColumnMajor(xF64 d[][3]) const
	{
	//we are column major, so copy transposed.
	d[0][0] = (xF64)data.s._11;
	d[1][0] = (xF64)data.s._12;
	d[2][0] = (xF64)data.s._13;

	d[0][1] = (xF64)data.s._21;
	d[1][1] = (xF64)data.s._22;
	d[2][1] = (xF64)data.s._23;

	d[0][2] = (xF64)data.s._31;
	d[1][2] = (xF64)data.s._32;
	d[2][2] = (xF64)data.s._33;
	}


X_INLINE void xMat33::setRowMajorStride4(const xF64*d)
	{
	//we are also row major, so this is a direct copy
	//however we've got to skip every 4th element.
	data.s._11 = (xReal)d[0];
	data.s._12 = (xReal)d[1];
	data.s._13 = (xReal)d[2];

	data.s._21 = (xReal)d[4];
	data.s._22 = (xReal)d[5];
	data.s._23 = (xReal)d[6];

	data.s._31 = (xReal)d[8];
	data.s._32 = (xReal)d[9];
	data.s._33 = (xReal)d[10];
	}


X_INLINE void xMat33::setRowMajorStride4(const xF64 d[][4])
	{
	//we are also row major, so this is a direct copy
	data.s._11 = (xReal)d[0][0];
	data.s._12 = (xReal)d[0][1];
	data.s._13 = (xReal)d[0][2];

	data.s._21 = (xReal)d[1][0];
	data.s._22 = (xReal)d[1][1];
	data.s._23 = (xReal)d[1][2];

	data.s._31 = (xReal)d[2][0];
	data.s._32 = (xReal)d[2][1];
	data.s._33 = (xReal)d[2][2];
	}


X_INLINE void xMat33::setColumnMajorStride4(const xF64*d)
	{
	//we are column major, so copy transposed.
	//however we've got to skip every 4th element.
	data.s._11 = (xReal)d[0];
	data.s._12 = (xReal)d[4];
	data.s._13 = (xReal)d[8];

	data.s._21 = (xReal)d[1];
	data.s._22 = (xReal)d[5];
	data.s._23 = (xReal)d[9];

	data.s._31 = (xReal)d[2];
	data.s._32 = (xReal)d[6];
	data.s._33 = (xReal)d[10];
	}


X_INLINE void xMat33::setColumnMajorStride4(const xF64 d[][4])
	{
	//we are column major, so copy transposed.
	data.s._11 = (xReal)d[0][0];
	data.s._12 = (xReal)d[1][0];
	data.s._13 = (xReal)d[2][0];

	data.s._21 = (xReal)d[0][1];
	data.s._22 = (xReal)d[1][1];
	data.s._23 = (xReal)d[2][1];

	data.s._31 = (xReal)d[0][2];
	data.s._32 = (xReal)d[1][2];
	data.s._33 = (xReal)d[2][2];
	}


X_INLINE void xMat33::getRowMajorStride4(xF64*d) const
	{
	//we are also row major, so this is a direct copy
	//however we've got to skip every 4th element.
	d[0] = (xF64)data.s._11;
	d[1] = (xF64)data.s._12;
	d[2] = (xF64)data.s._13;

	d[4] = (xF64)data.s._21;
	d[5] = (xF64)data.s._22;
	d[6] = (xF64)data.s._23;

	d[8] = (xF64)data.s._31;
	d[9] = (xF64)data.s._32;
	d[10]= (xF64)data.s._33;
	}


X_INLINE void xMat33::getRowMajorStride4(xF64 d[][4]) const
	{
	//we are also row major, so this is a direct copy
	d[0][0] = (xF64)data.s._11;
	d[0][1] = (xF64)data.s._12;
	d[0][2] = (xF64)data.s._13;

	d[1][0] = (xF64)data.s._21;
	d[1][1] = (xF64)data.s._22;
	d[1][2] = (xF64)data.s._23;

	d[2][0] = (xF64)data.s._31;
	d[2][1] = (xF64)data.s._32;
	d[2][2] = (xF64)data.s._33;
	}


X_INLINE void xMat33::getColumnMajorStride4(xF64*d) const

	{
	//we are column major, so copy transposed.
	//however we've got to skip every 4th element.
	d[0] = (xF64)data.s._11;
	d[4] = (xF64)data.s._12;
	d[8] = (xF64)data.s._13;

	d[1] = (xF64)data.s._21;
	d[5] = (xF64)data.s._22;
	d[9] = (xF64)data.s._23;

	d[2] = (xF64)data.s._31;
	d[6] = (xF64)data.s._32;
	d[10]= (xF64)data.s._33;
	}


X_INLINE void xMat33::getColumnMajorStride4(xF64 d[][4]) const
	{
	//we are column major, so copy transposed.
	d[0][0] = (xF64)data.s._11;
	d[1][0] = (xF64)data.s._12;
	d[2][0] = (xF64)data.s._13;

	d[0][1] = (xF64)data.s._21;
	d[1][1] = (xF64)data.s._22;
	d[2][1] = (xF64)data.s._23;

	d[0][2] = (xF64)data.s._31;
	d[1][2] = (xF64)data.s._32;
	d[2][2] = (xF64)data.s._33;
	}


X_INLINE void xMat33::setRow(int row, const xVec3 & v)
	{
#ifndef TRANSPOSED_MAT33
	data.m[row][0] = v.x;
	data.m[row][1] = v.y;
	data.m[row][2] = v.z;
#else
	data.m[0][row] = v.x;
	data.m[1][row] = v.y;
	data.m[2][row] = v.z;
#endif
	}


X_INLINE void xMat33::setColumn(int col, const xVec3 & v)
	{
#ifndef TRANSPOSED_MAT33
	data.m[0][col] = v.x;
	data.m[1][col] = v.y;
	data.m[2][col] = v.z;
#else
	data.m[col][0] = v.x;
	data.m[col][1] = v.y;
	data.m[col][2] = v.z;
#endif
	}


X_INLINE void xMat33::getRow(int row, xVec3 & v) const
	{
#ifndef TRANSPOSED_MAT33
	v.x = data.m[row][0];
	v.y = data.m[row][1];
	v.z = data.m[row][2];
#else
	v.x = data.m[0][row];
	v.y = data.m[1][row];
	v.z = data.m[2][row];
#endif
	}


X_INLINE void xMat33::getColumn(int col, xVec3 & v) const
	{
#ifndef TRANSPOSED_MAT33
	v.x = data.m[0][col];
	v.y = data.m[1][col];
	v.z = data.m[2][col];
#else
	v.x = data.m[col][0];
	v.y = data.m[col][1];
	v.z = data.m[col][2];
#endif
	}


X_INLINE xVec3 xMat33::getRow(int row) const
{
#ifndef TRANSPOSED_MAT33
	return xVec3(data.m[row][0],data.m[row][1],data.m[row][2]);
#else
	return xVec3(data.m[0][row],data.m[1][row],data.m[2][row]);
#endif
}

X_INLINE xVec3 xMat33::getColumn(int col) const
{
#ifndef TRANSPOSED_MAT33
	return xVec3(data.m[0][col],data.m[1][col],data.m[2][col]);
#else
	return xVec3(data.m[col][0],data.m[col][1],data.m[col][2]);
#endif
}

X_INLINE xReal & xMat33::operator()(int row, int col)
	{
#ifndef TRANSPOSED_MAT33
	return data.m[row][col];
#else
	return data.m[col][row];
#endif
	}


X_INLINE const xReal & xMat33::operator() (int row, int col) const
	{
#ifndef TRANSPOSED_MAT33
	return data.m[row][col];
#else
	return data.m[col][row];
#endif
	}

//const methods


X_INLINE bool xMat33::isIdentity() const
	{
	if(data.s._11 != 1.0f)		return false;
	if(data.s._12 != 0.0f)		return false;
	if(data.s._13 != 0.0f)		return false;

	if(data.s._21 != 0.0f)		return false;
	if(data.s._22 != 1.0f)		return false;
	if(data.s._23 != 0.0f)		return false;

	if(data.s._31 != 0.0f)		return false;
	if(data.s._32 != 0.0f)		return false;
	if(data.s._33 != 1.0f)		return false;

	return true;
	}


X_INLINE bool xMat33::isZero() const
	{
	if(data.s._11 != 0.0f)		return false;
	if(data.s._12 != 0.0f)		return false;
	if(data.s._13 != 0.0f)		return false;

	if(data.s._21 != 0.0f)		return false;
	if(data.s._22 != 0.0f)		return false;
	if(data.s._23 != 0.0f)		return false;

	if(data.s._31 != 0.0f)		return false;
	if(data.s._32 != 0.0f)		return false;
	if(data.s._33 != 0.0f)		return false;

	return true;
	}


X_INLINE bool xMat33::isFinite() const
	{
	return xMath::isFinite(data.s._11)
	&& xMath::isFinite(data.s._12)
	&& xMath::isFinite(data.s._13)

	&& xMath::isFinite(data.s._21)
	&& xMath::isFinite(data.s._22)
	&& xMath::isFinite(data.s._23)

	&& xMath::isFinite(data.s._31)
	&& xMath::isFinite(data.s._32)
	&& xMath::isFinite(data.s._33);
	}



X_INLINE void xMat33::zero()
	{
	data.s._11 = xReal(0.0);
	data.s._12 = xReal(0.0);
	data.s._13 = xReal(0.0);

	data.s._21 = xReal(0.0);
	data.s._22 = xReal(0.0);
	data.s._23 = xReal(0.0);

	data.s._31 = xReal(0.0);
	data.s._32 = xReal(0.0);
	data.s._33 = xReal(0.0);
	}


X_INLINE void xMat33::id()
	{
	data.s._11 = xReal(1.0);
	data.s._12 = xReal(0.0);
	data.s._13 = xReal(0.0);

	data.s._21 = xReal(0.0);
	data.s._22 = xReal(1.0);
	data.s._23 = xReal(0.0);

	data.s._31 = xReal(0.0);
	data.s._32 = xReal(0.0);
	data.s._33 = xReal(1.0);
	}


X_INLINE void xMat33::setNegative()
	{
	data.s._11 = -data.s._11;
	data.s._12 = -data.s._12;
	data.s._13 = -data.s._13;

	data.s._21 = -data.s._21;
	data.s._22 = -data.s._22;
	data.s._23 = -data.s._23;

	data.s._31 = -data.s._31;
	data.s._32 = -data.s._32;
	data.s._33 = -data.s._33;
	}


X_INLINE void xMat33::diagonal(const xVec3 &v)
	{
	data.s._11 = v.x;
	data.s._12 = xReal(0.0);
	data.s._13 = xReal(0.0);

	data.s._21 = xReal(0.0);
	data.s._22 = v.y;
	data.s._23 = xReal(0.0);

	data.s._31 = xReal(0.0);
	data.s._32 = xReal(0.0);
	data.s._33 = v.z;
	}


X_INLINE void xMat33::star(const xVec3 &v)
	{
	data.s._11 = xReal(0.0);	data.s._12 =-v.z;	data.s._13 = v.y;
	data.s._21 = v.z;	data.s._22 = xReal(0.0);	data.s._23 =-v.x;
	data.s._31 =-v.y;	data.s._32 = v.x;	data.s._33 = xReal(0.0);
	}


X_INLINE void xMat33::fromQuat(const xQuat & q)
	{
	const xReal w = q.w;
	const xReal x = q.x;
	const xReal y = q.y;
	const xReal z = q.z;

	data.s._11 = xReal(1.0) - y*y*xReal(2.0) - z*z*xReal(2.0);
	data.s._12 = x*y*xReal(2.0) - w*z*xReal(2.0);	
	data.s._13 = x*z*xReal(2.0) + w*y*xReal(2.0);	

	data.s._21 = x*y*xReal(2.0) + w*z*xReal(2.0);	
	data.s._22 = xReal(1.0) - x*x*xReal(2.0) - z*z*xReal(2.0);	
	data.s._23 = y*z*xReal(2.0) - w*x*xReal(2.0);	
	
	data.s._31 = x*z*xReal(2.0) - w*y*xReal(2.0);	
	data.s._32 = y*z*xReal(2.0) + w*x*xReal(2.0);	
	data.s._33 = xReal(1.0) - x*x*xReal(2.0) - y*y*xReal(2.0);	
	}


X_INLINE void xMat33::toQuat(xQuat & q) const					// set the xQuat from a rotation matrix
	{
    xReal tr, s;
    tr = data.s._11 + data.s._22 + data.s._33;
    if(tr >= 0)
		{
		s = (xReal)xMath::sqrt(tr +1);
		q.w = xReal(0.5) * s;
		s = xReal(0.5) / s;
		q.x = ((*this)(2,1) - (*this)(1,2)) * s;
		q.y = ((*this)(0,2) - (*this)(2,0)) * s;
		q.z = ((*this)(1,0) - (*this)(0,1)) * s;
		}
    else
		{
		int i = 0; 
		if (data.s._22 > data.s._11)
			i = 1; 
		if(data.s._33 > (*this)(i,i))
			i=2; 
		switch (i)
			{
			case 0:
				s = (xReal)xMath::sqrt((data.s._11 - (data.s._22 + data.s._33)) + 1);
				q.x = xReal(0.5) * s;
				s = xReal(0.5) / s;
				q.y = ((*this)(0,1) + (*this)(1,0)) * s; 
				q.z = ((*this)(2,0) + (*this)(0,2)) * s;
				q.w = ((*this)(2,1) - (*this)(1,2)) * s;
				break;
			case 1:
				s = (xReal)xMath::sqrt((data.s._22 - (data.s._33 + data.s._11)) + 1);
				q.y = xReal(0.5) * s;
				s = xReal(0.5) / s;
				q.z = ((*this)(1,2) + (*this)(2,1)) * s;
				q.x = ((*this)(0,1) + (*this)(1,0)) * s;
				q.w = ((*this)(0,2) - (*this)(2,0)) * s;
				break;
			case 2:
				s = (xReal)xMath::sqrt((data.s._33 - (data.s._11 + data.s._22)) + 1);
				q.z = xReal(0.5) * s;
				s = xReal(0.5) / s;
				q.x = ((*this)(2,0) + (*this)(0,2)) * s;
				q.y = ((*this)(1,2) + (*this)(2,1)) * s;
				q.w = ((*this)(1,0) - (*this)(0,1)) * s;
			}
		}
	}
/*

X_INLINE void xMat33::orthonormalize()	//Gram-Schmidt orthogonalization to correct numerical drift, plus column normalization
	{
	//TODO: This is buggy!
	xVec3 w,t1,t2,t3;
	xReal norm_sq;

    const xReal m=3;			//m := linalg[rowdim](A);
    const xReal n=3;			//n := linalg[coldim](A);
	int i, j, k = 0;				//k := 0;


    Mat33d v = *this;				//v := linalg[col](A, 1 .. n); -- 3 column vectors indexable
    xVec3 norm_u_sq;
																//# orthogonalize v[i]
    for (i=0; i<n; i++)//for i to n do
		{
        v.getColumn(i,w);		//i-th column
        for (j=0; j<k; j++)									//# pull w along projection of v[i] with u[j]
			{
			this->getColumn(j,t1);
			this->getColumn(j,t2);
			v.getColumn(i,t3);
			xVec3 temp = (t2 * (xReal(1.0)/norm_u_sq[j]));
			xVec3 temp2 = temp  * t3.dot( t1 );
			w -= temp;	
			}
																//        # compute norm of orthogonalized v[i]
      norm_sq = w.Dot(w);

		if (norm_sq != xReal(0.0)) 
			{													//           # linearly independent new orthogonal vector 
																//       # add to list of u and norm_u_sq
			this->SetColumn(i,w);									//u = [op(u), evalm(w)];
            norm_u_sq[i] = norm_sq;						//norm_u_sq = [op(norm_u_sq), norm_sq];
            k ++;
			}
		}
	

	xVec3 temp;													//may want to do this in-place -- dunno if optimizer does this for me
	for (i=0; i<3; i++)
		{
		getColumn(i,temp);
		temp.normalize();
		setColumn(i,temp);
		}
	}
	*/


X_INLINE void xMat33::setTransposed(const xMat33& other)
	{
	//gotta special case in-place case
	if (this != &other)
		{
		data.s._11 = other.data.s._11;
		data.s._12 = other.data.s._21;
		data.s._13 = other.data.s._31;

		data.s._21 = other.data.s._12;
		data.s._22 = other.data.s._22;
		data.s._23 = other.data.s._32;

		data.s._31 = other.data.s._13;
		data.s._32 = other.data.s._23;
		data.s._33 = other.data.s._33;
		}
	else
		{
		xReal tx, ty, tz;
		tx = data.s._21;	data.s._21 = other.data.s._12;	data.s._12 = tx;
		ty = data.s._31;	data.s._31 = other.data.s._13;	data.s._13 = ty;
		tz = data.s._32;	data.s._32 = other.data.s._23;	data.s._23 = tz;
		}
	}


X_INLINE void xMat33::setTransposed()
	{
		X_Swap(data.s._12, data.s._21);
		X_Swap(data.s._23, data.s._32);
		X_Swap(data.s._13, data.s._31);
	}


X_INLINE void xMat33::multiplyDiagonal(const xVec3 &d)
	{
	data.s._11 *= d.x;
	data.s._12 *= d.y;
	data.s._13 *= d.z;

	data.s._21 *= d.x;
	data.s._22 *= d.y;
	data.s._23 *= d.z;

	data.s._31 *= d.x;
	data.s._32 *= d.y;
	data.s._33 *= d.z;
	}


X_INLINE void xMat33::multiplyDiagonalTranspose(const xVec3 &d)
	{
		xReal temp;
		data.s._11 = data.s._11 * d.x;
		data.s._22 = data.s._22 * d.y;
		data.s._33 = data.s._33 * d.z;

		temp = data.s._21 * d.y;
		data.s._21 = data.s._12 * d.x;
		data.s._12 = temp;

		temp = data.s._31 * d.z;
		data.s._31 = data.s._13 * d.x;
		data.s._13 = temp;
		
		temp = data.s._32 * d.z;
		data.s._32 = data.s._23 * d.y;
		data.s._23 = temp;
	}


X_INLINE void xMat33::multiplyDiagonal(const xVec3 &d, xMat33& dst) const
	{
	dst.data.s._11 = data.s._11 * d.x;
	dst.data.s._12 = data.s._12 * d.y;
	dst.data.s._13 = data.s._13 * d.z;

	dst.data.s._21 = data.s._21 * d.x;
	dst.data.s._22 = data.s._22 * d.y;
	dst.data.s._23 = data.s._23 * d.z;

	dst.data.s._31 = data.s._31 * d.x;
	dst.data.s._32 = data.s._32 * d.y;
	dst.data.s._33 = data.s._33 * d.z;
	}


X_INLINE void xMat33::multiplyDiagonalTranspose(const xVec3 &d, xMat33& dst) const
	{
	dst.data.s._11 = data.s._11 * d.x;
	dst.data.s._12 = data.s._21 * d.y;
	dst.data.s._13 = data.s._31 * d.z;

	dst.data.s._21 = data.s._12 * d.x;
	dst.data.s._22 = data.s._22 * d.y;
	dst.data.s._23 = data.s._32 * d.z;

	dst.data.s._31 = data.s._13 * d.x;
	dst.data.s._32 = data.s._23 * d.y;
	dst.data.s._33 = data.s._33 * d.z;
	}


X_INLINE void xMat33::multiply(const xVec3 &src, xVec3 &dst) const
	{
	xReal x,y,z;	//so it works if src == dst
	x = data.s._11 * src.x + data.s._12 * src.y + data.s._13 * src.z;
	y = data.s._21 * src.x + data.s._22 * src.y + data.s._23 * src.z;
	z = data.s._31 * src.x + data.s._32 * src.y + data.s._33 * src.z;

	dst.x = x;
	dst.y = y;
	dst.z = z;	
	}


X_INLINE void xMat33::multiplyByTranspose(const xVec3 &src, xVec3 &dst) const
	{
	xReal x,y,z;	//so it works if src == dst
	x = data.s._11 * src.x + data.s._21 * src.y + data.s._31 * src.z;
	y = data.s._12 * src.x + data.s._22 * src.y + data.s._32 * src.z;
	z = data.s._13 * src.x + data.s._23 * src.y + data.s._33 * src.z;

	dst.x = x;
	dst.y = y;
	dst.z = z;	
	}


X_INLINE void xMat33::add(const xMat33 & a, const xMat33 & b)
	{
	data.s._11 = a.data.s._11 + b.data.s._11;
	data.s._12 = a.data.s._12 + b.data.s._12;
	data.s._13 = a.data.s._13 + b.data.s._13;

	data.s._21 = a.data.s._21 + b.data.s._21;
	data.s._22 = a.data.s._22 + b.data.s._22;
	data.s._23 = a.data.s._23 + b.data.s._23;

	data.s._31 = a.data.s._31 + b.data.s._31;
	data.s._32 = a.data.s._32 + b.data.s._32;
	data.s._33 = a.data.s._33 + b.data.s._33;
	}


X_INLINE void xMat33::subtract(const xMat33 &a, const xMat33 &b)
	{
	data.s._11 = a.data.s._11 - b.data.s._11;
	data.s._12 = a.data.s._12 - b.data.s._12;
	data.s._13 = a.data.s._13 - b.data.s._13;

	data.s._21 = a.data.s._21 - b.data.s._21;
	data.s._22 = a.data.s._22 - b.data.s._22;
	data.s._23 = a.data.s._23 - b.data.s._23;

	data.s._31 = a.data.s._31 - b.data.s._31;
	data.s._32 = a.data.s._32 - b.data.s._32;
	data.s._33 = a.data.s._33 - b.data.s._33;
	}


X_INLINE void xMat33::multiply(xReal d,  const xMat33 & a)
	{
	data.s._11 = a.data.s._11 * d;
	data.s._12 = a.data.s._12 * d;
	data.s._13 = a.data.s._13 * d;

	data.s._21 = a.data.s._21 * d;
	data.s._22 = a.data.s._22 * d;
	data.s._23 = a.data.s._23 * d;

	data.s._31 = a.data.s._31 * d;
	data.s._32 = a.data.s._32 * d;
	data.s._33 = a.data.s._33 * d;
	}


X_INLINE void xMat33::multiply(const xMat33& left, const xMat33& right)
	{
	xReal a,b,c, d,e,f, g,h,i;
	//note: temps needed so that x.multiply(x,y) works OK.
	a =left.data.s._11 * right.data.s._11 +left.data.s._12 * right.data.s._21 +left.data.s._13 * right.data.s._31;
	b =left.data.s._11 * right.data.s._12 +left.data.s._12 * right.data.s._22 +left.data.s._13 * right.data.s._32;
	c =left.data.s._11 * right.data.s._13 +left.data.s._12 * right.data.s._23 +left.data.s._13 * right.data.s._33;

	d =left.data.s._21 * right.data.s._11 +left.data.s._22 * right.data.s._21 +left.data.s._23 * right.data.s._31;
	e =left.data.s._21 * right.data.s._12 +left.data.s._22 * right.data.s._22 +left.data.s._23 * right.data.s._32;
	f =left.data.s._21 * right.data.s._13 +left.data.s._22 * right.data.s._23 +left.data.s._23 * right.data.s._33;

	g =left.data.s._31 * right.data.s._11 +left.data.s._32 * right.data.s._21 +left.data.s._33 * right.data.s._31;
	h =left.data.s._31 * right.data.s._12 +left.data.s._32 * right.data.s._22 +left.data.s._33 * right.data.s._32;
	i =left.data.s._31 * right.data.s._13 +left.data.s._32 * right.data.s._23 +left.data.s._33 * right.data.s._33;


	data.s._11 = a;
	data.s._12 = b;
	data.s._13 = c;

	data.s._21 = d;
	data.s._22 = e;
	data.s._23 = f;

	data.s._31 = g;
	data.s._32 = h;
	data.s._33 = i;
	}


X_INLINE void xMat33::multiplyTransposeLeft(const xMat33& left, const xMat33& right)
	{
	xReal a,b,c, d,e,f, g,h,i;
	//note: temps needed so that x.multiply(x,y) works OK.
	a =left.data.s._11 * right.data.s._11 +left.data.s._21 * right.data.s._21 +left.data.s._31 * right.data.s._31;
	b =left.data.s._11 * right.data.s._12 +left.data.s._21 * right.data.s._22 +left.data.s._31 * right.data.s._32;
	c =left.data.s._11 * right.data.s._13 +left.data.s._21 * right.data.s._23 +left.data.s._31 * right.data.s._33;

	d =left.data.s._12 * right.data.s._11 +left.data.s._22 * right.data.s._21 +left.data.s._32 * right.data.s._31;
	e =left.data.s._12 * right.data.s._12 +left.data.s._22 * right.data.s._22 +left.data.s._32 * right.data.s._32;
	f =left.data.s._12 * right.data.s._13 +left.data.s._22 * right.data.s._23 +left.data.s._32 * right.data.s._33;

	g =left.data.s._13 * right.data.s._11 +left.data.s._23 * right.data.s._21 +left.data.s._33 * right.data.s._31;
	h =left.data.s._13 * right.data.s._12 +left.data.s._23 * right.data.s._22 +left.data.s._33 * right.data.s._32;
	i =left.data.s._13 * right.data.s._13 +left.data.s._23 * right.data.s._23 +left.data.s._33 * right.data.s._33;

	data.s._11 = a;
	data.s._12 = b;
	data.s._13 = c;

	data.s._21 = d;
	data.s._22 = e;
	data.s._23 = f;

	data.s._31 = g;
	data.s._32 = h;
	data.s._33 = i;
	}


X_INLINE void xMat33::multiplyTransposeRight(const xMat33& left, const xMat33& right)
	{
	xReal a,b,c, d,e,f, g,h,i;
	//note: temps needed so that x.multiply(x,y) works OK.
	a =left.data.s._11 * right.data.s._11 +left.data.s._12 * right.data.s._12 +left.data.s._13 * right.data.s._13;
	b =left.data.s._11 * right.data.s._21 +left.data.s._12 * right.data.s._22 +left.data.s._13 * right.data.s._23;
	c =left.data.s._11 * right.data.s._31 +left.data.s._12 * right.data.s._32 +left.data.s._13 * right.data.s._33;

	d =left.data.s._21 * right.data.s._11 +left.data.s._22 * right.data.s._12 +left.data.s._23 * right.data.s._13;
	e =left.data.s._21 * right.data.s._21 +left.data.s._22 * right.data.s._22 +left.data.s._23 * right.data.s._23;
	f =left.data.s._21 * right.data.s._31 +left.data.s._22 * right.data.s._32 +left.data.s._23 * right.data.s._33;

	g =left.data.s._31 * right.data.s._11 +left.data.s._32 * right.data.s._12 +left.data.s._33 * right.data.s._13;
	h =left.data.s._31 * right.data.s._21 +left.data.s._32 * right.data.s._22 +left.data.s._33 * right.data.s._23;
	i =left.data.s._31 * right.data.s._31 +left.data.s._32 * right.data.s._32 +left.data.s._33 * right.data.s._33;

	data.s._11 = a;
	data.s._12 = b;
	data.s._13 = c;

	data.s._21 = d;
	data.s._22 = e;
	data.s._23 = f;

	data.s._31 = g;
	data.s._32 = h;
	data.s._33 = i;
	}


X_INLINE void xMat33::multiplyTransposeRight(const xVec3 &left, const xVec3 &right)
	{
	data.s._11 = left.x * right.x;
	data.s._12 = left.x * right.y;
	data.s._13 = left.x * right.z;

	data.s._21 = left.y * right.x;
	data.s._22 = left.y * right.y;
	data.s._23 = left.y * right.z;

	data.s._31 = left.z * right.x;
	data.s._32 = left.z * right.y;
	data.s._33 = left.z * right.z;
	}

X_INLINE void xMat33::rotX(xReal angle)
	{
	xReal Cos = cosf(angle);
	xReal Sin = sinf(angle);
	id();
	data.m[1][1] = data.m[2][2] = Cos;
	data.m[1][2] = -Sin;
	data.m[2][1] = Sin;
	}

X_INLINE void xMat33::rotY(xReal angle)
	{
	xReal Cos = cosf(angle);
	xReal Sin = sinf(angle);
	id();
	data.m[0][0] = data.m[2][2] = Cos;
	data.m[0][2] = Sin;
	data.m[2][0] = -Sin;
	}

X_INLINE void xMat33::rotZ(xReal angle)
	{
	xReal Cos = cosf(angle);
	xReal Sin = sinf(angle);
	id();
	data.m[0][0] = data.m[1][1] = Cos;
	data.m[0][1] = -Sin;
	data.m[1][0] = Sin;
	}

X_INLINE xVec3  xMat33::operator%(const xVec3 & src) const
	{
	xVec3 dest;
	this->multiplyByTranspose(src, dest);
	return dest;
	}


X_INLINE xVec3  xMat33::operator*(const xVec3 & src) const
	{
	xVec3 dest;
	this->multiply(src, dest);
	return dest;
	}


X_INLINE const xMat33 &xMat33::operator +=(const xMat33 &d)
	{
	data.s._11 += d.data.s._11;
	data.s._12 += d.data.s._12;
	data.s._13 += d.data.s._13;

	data.s._21 += d.data.s._21;
	data.s._22 += d.data.s._22;
	data.s._23 += d.data.s._23;

	data.s._31 += d.data.s._31;
	data.s._32 += d.data.s._32;
	data.s._33 += d.data.s._33;
	return *this;
	}


X_INLINE const xMat33 &xMat33::operator -=(const xMat33 &d)
	{
	data.s._11 -= d.data.s._11;
	data.s._12 -= d.data.s._12;
	data.s._13 -= d.data.s._13;

	data.s._21 -= d.data.s._21;
	data.s._22 -= d.data.s._22;
	data.s._23 -= d.data.s._23;

	data.s._31 -= d.data.s._31;
	data.s._32 -= d.data.s._32;
	data.s._33 -= d.data.s._33;
	return *this;
	}


X_INLINE const xMat33 &xMat33::operator *=(xReal f)
	{
	data.s._11 *= f;
	data.s._12 *= f;
	data.s._13 *= f;

	data.s._21 *= f;
	data.s._22 *= f;
	data.s._23 *= f;

	data.s._31 *= f;
	data.s._32 *= f;
	data.s._33 *= f;
	return *this;
	}


X_INLINE const xMat33 &xMat33::operator /=(xReal x)
	{
	xReal f = xReal(1.0) / x;
	data.s._11 *= f;
	data.s._12 *= f;
	data.s._13 *= f;

	data.s._21 *= f;
	data.s._22 *= f;
	data.s._23 *= f;

	data.s._31 *= f;
	data.s._32 *= f;
	data.s._33 *= f;
	return *this;
	}


X_INLINE xReal xMat33::determinant() const
	{
	return  data.s._11*data.s._22*data.s._33 + data.s._12*data.s._23*data.s._31 + data.s._13*data.s._21*data.s._32 
		  - data.s._13*data.s._22*data.s._31 - data.s._12*data.s._21*data.s._33 - data.s._11*data.s._23*data.s._32;
	}


bool xMat33::getInverse(xMat33& dest) const
	{
	xReal b00,b01,b02,b10,b11,b12,b20,b21,b22;

	b00 = data.s._22*data.s._33-data.s._23*data.s._32;	b01 = data.s._13*data.s._32-data.s._12*data.s._33;	b02 = data.s._12*data.s._23-data.s._13*data.s._22;
	b10 = data.s._23*data.s._31-data.s._21*data.s._33;	b11 = data.s._11*data.s._33-data.s._13*data.s._31;	b12 = data.s._13*data.s._21-data.s._11*data.s._23;
	b20 = data.s._21*data.s._32-data.s._22*data.s._31;	b21 = data.s._12*data.s._31-data.s._11*data.s._32;	b22 = data.s._11*data.s._22-data.s._12*data.s._21;
	


	/*
	compute determinant: 
	xReal d =   a00*a11*a22 + a01*a12*a20 + a02*a10*a21	- a02*a11*a20 - a01*a10*a22 - a00*a12*a21;
				0				1			2			3				4			5

	this is a subset of the multiplies done above:

	xReal d = b00*a00				+		b01*a10						 + b02 * a20;
	xReal d = (a11*a22-a12*a21)*a00 +		(a02*a21-a01*a22)*a10		 + (a01*a12-a02*a11) * a20;

	xReal d = a11*a22*a00-a12*a21*a00 +		a02*a21*a10-a01*a22*a10		 + a01*a12*a20-a02*a11*a20;
			0			5					2			4					1			3
	*/

	xReal d = b00*data.s._11		+		b01*data.s._21				 + b02 * data.s._31;
	
	if (d == xReal(0.0))		//singular?
		{
		dest.id();
		return false;
		}
	
	d = xReal(1.0)/d;

	//only do assignment at the end, in case dest == this:


	dest.data.s._11 = b00*d; dest.data.s._12 = b01*d; dest.data.s._13 = b02*d;
	dest.data.s._21 = b10*d; dest.data.s._22 = b11*d; dest.data.s._23 = b12*d;
	dest.data.s._31 = b20*d; dest.data.s._32 = b21*d; dest.data.s._33 = b22*d;

	return true;
	}


X_INLINE xMat33&	xMat33::operator*= (const xMat33& mat)
	{
	this->multiply(*this, mat);
	return *this;
	}


X_INLINE xMat33	xMat33::operator-  (const xMat33& mat)	const
	{
	xMat33 temp;
	temp.subtract(*this, mat);
	return temp;
	}


X_INLINE xMat33	xMat33::operator+  (const xMat33& mat)	const
	{
	xMat33 temp;
	temp.add(*this, mat);
	return temp;
	}


X_INLINE xMat33	xMat33::operator*  (const xMat33& mat)	const
	{
	xMat33 temp;
	temp.multiply(*this, mat);
	return temp;
	}


X_INLINE xMat33	xMat33::operator*  (float s)			const
	{
	xMat33 temp;
	temp.multiply(s, *this);
	return temp;
	}

X_INLINE xQuat::xQuat(const class xMat33 &m)
{
	m.toQuat(*this);
}

 /** @} */
#endif
//AGCOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 AGEIA Technologies.
// All rights reserved. www.ageia.com
///////////////////////////////////////////////////////////////////////////
//AGCOPYRIGHTEND
