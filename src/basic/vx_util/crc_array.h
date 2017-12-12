// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
// ** Copyright UCAR (c) 1992 - 2014
// ** University Corporation for Atmospheric Research (UCAR)
// ** National Center for Atmospheric Research (NCAR)
// ** Research Applications Lab (RAL)
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*



////////////////////////////////////////////////////////////////////////


#ifndef  __CONTIGUOUS_RETURNS_COPY_ARRAY_H__
#define  __CONTIGUOUS_RETURNS_COPY_ARRAY_H__


////////////////////////////////////////////////////////////////////////


#include <iostream>

#include "num_array.h"
#include "int_array.h"
#include "is_bad_data.h"
#include "nint.h"
#include "vx_cal.h"
#include "vx_log.h"


////////////////////////////////////////////////////////////////////////


static const int crc_array_alloc_inc = 25;


////////////////////////////////////////////////////////////////////////


template <typename T>

int is_bigger(const void * p1, const void * p2)

{

const T & a = *((T *) p1);
const T & b = *((T *) p2);

if ( a < b )  return ( -1 );
if ( a > b )  return (  1 );

return ( 0 );

}



////////////////////////////////////////////////////////////////////////


template <typename T>

class CRC_Array {

   private:

      void init_from_scratch();

      void assign(const CRC_Array &);


      T * e;

      int Nelements;

      int Nalloc;

   public:

      CRC_Array() { init_from_scratch(); }

     ~CRC_Array() { clear(); }

      CRC_Array(const CRC_Array <T> & _a)  { init_from_scratch();  assign(_a); }

      CRC_Array & operator=(const CRC_Array <T> & _a)  {

         if ( this == &_a )  return ( * this );

         assign(_a);

         return ( * this );

      }

      CRC_Array <T> & operator=(const NumArray &);

      void clear();

      void extend(int);

      void dump(ostream &, int depth = 0) const;

      void dump_one_line(ostream & out, int depth) const;

         //
         //  set stuff
         //


         //
         //  get stuff
         //

      int n_elements() const { return ( Nelements ); }
      int n         () const { return ( Nelements ); }

      T operator[] (int) const;

         //
         //  do stuff
         //

      bool has(const T &) const;
      bool has(const T &, int & index) const;

      void add(const T &);
      void add(const CRC_Array <T> &);
      void add_css_sec(const char *);

      void set(unsigned int ix, const T & val);

      void sort_increasing();

      void increment(const T &);   //  adds a constant value to all elements

      T sum() const;
      T min() const;
      T max() const;

};


////////////////////////////////////////////////////////////////////////


// static int is_bigger(const void *, const void *);


////////////////////////////////////////////////////////////////////////


   //
   //  Code for class CRC_Array
   //


////////////////////////////////////////////////////////////////////////


template <typename T>

CRC_Array <T> & CRC_Array<T>::operator=(const NumArray & a)

{

clear();

for(int j=0; j<a.n_elements(); ++j) add(nint(a[j]));

return ( * this );

}


////////////////////////////////////////////////////////////////////////


template <typename T>

void CRC_Array<T>::init_from_scratch()

{

e = (T *) 0;

clear();

return;

}


////////////////////////////////////////////////////////////////////////


template <typename T>

void CRC_Array<T>::clear()

{

if ( e )  { delete [] e;  e = (T *) 0; }

Nelements = Nalloc = 0;

return;

}


////////////////////////////////////////////////////////////////////////


template <typename T>

void CRC_Array<T>::assign(const CRC_Array <T> & a)

{

clear();

if ( a.Nelements == 0 )  return;

extend(a.Nelements);

int j;

for (j=0; j<(a.Nelements); ++j)  {

   e[j] = a.e[j];

}

Nelements = a.Nelements;


return;

}


////////////////////////////////////////////////////////////////////////


template <typename T>

void CRC_Array<T>::extend(int n)

{

if ( Nalloc >= n )  return;

int k;

k = n/crc_array_alloc_inc;

if ( n%crc_array_alloc_inc )  ++k;

n = k*crc_array_alloc_inc;

T * u = (T *) 0;

u = new T [n];

if ( !u )  {

   mlog << Error << "\nvoid CRC_Array::extend(int) -> memory allocation error\n\n";

   exit ( 1 );

}

int j;

memset(u, 0, n*sizeof(T));

if ( e )  {

   for (j=0; j<Nelements; ++j)  {

      u[j] = e[j];

   }

   delete [] e;  e = (T *) 0;

}

e = u;   u = (T *) 0;

Nalloc = n;



return;

}


////////////////////////////////////////////////////////////////////////


template <typename T>

void CRC_Array<T>::dump(ostream & out, int depth) const

{

Indent prefix(depth);


out << prefix << "Nelements = " << Nelements << "\n";
out << prefix << "Nalloc    = " << Nalloc    << "\n";

int j;

for (j=0; j<Nelements; ++j)  {

   out << prefix << "Element # " << j << " = " << e[j] << "\n";

}




   //
   //  done
   //

out.flush();

return;

}


////////////////////////////////////////////////////////////////////////


template <typename T>

void CRC_Array<T>::dump_one_line(ostream & out, int depth) const

{

int j;
Indent prefix(depth);

out << prefix << '(' << Nelements << ") ";

for (j=0; j<Nelements; ++j)  {

   if ( j > 0 )  out << ' ';

   out << e[j];

}

   //
   //  done
   //

out << '\n' << flush;

return;

}


////////////////////////////////////////////////////////////////////////
template <typename T>
void CRC_Array<T>::set(unsigned int ix, const T & elem)
{
	if ( (ix >= Nelements) )  {
		mlog << Error << "\nCRC_Array::set(unsigned int, T) const -> range check error\n\n";
		exit ( 1 );
	}
	e[ix] = elem;
}

////////////////////////////////////////////////////////////////////////


template <typename T>

T CRC_Array<T>::operator[](int n) const

{

if ( (n < 0) || (n >= Nelements) )  {

   mlog << Error << "\nCRC_Array::operator[](int) const -> range check error\n\n";

   exit ( 1 );

}

return ( e[n] );

}


////////////////////////////////////////////////////////////////////////


template <typename T>

bool CRC_Array<T>::has(const T & k) const

{

int j;

for (j=0; j<Nelements; ++j)  {

   if ( e[j] == k )  return ( true );

}

return ( false );

}


////////////////////////////////////////////////////////////////////////


template <typename T>

bool CRC_Array<T>::has(const T & k, int & index) const

{

int j;

index = -1;

for (j=0; j<Nelements; ++j)  {

   if ( e[j] == k )  { index = j;  return ( true ); }

}

return ( false );

}


////////////////////////////////////////////////////////////////////////


template <typename T>

void CRC_Array<T>::add(const T & k)

{

extend(Nelements + 1);

e[Nelements++] = k;

return;

}


////////////////////////////////////////////////////////////////////////


template <typename T>

void CRC_Array<T>::add(const CRC_Array<T> & a)

{

extend(Nelements + a.Nelements);

int j;

for (j=0; j<(a.Nelements); ++j)  {

   e[Nelements++] = a.e[j];

}


return;

}


////////////////////////////////////////////////////////////////////////


template <typename T>

void CRC_Array<T>::add_css_sec(const char * text)

{

StringArray sa;

sa.parse_css(text);

extend(Nelements + sa.n_elements());

int j;

for (j=0; j<(sa.n_elements()); j++)  {

   add(timestring_to_sec(sa[j]));

}

return;

}


////////////////////////////////////////////////////////////////////////


template <typename T>

void CRC_Array<T>::sort_increasing()

{

if ( Nelements <= 1 )  return;

qsort(e, Nelements, sizeof(*e), is_bigger<T>);

return;

}


////////////////////////////////////////////////////////////////////////


template <typename T>

T CRC_Array<T>::sum() const

{

int j, count;
T s;

s = 0;

for(j=0, count=0; j<Nelements; j++) {

   if ( is_bad_data(e[j]) ) continue;

   s += e[j];

   count++;
}

if ( count == 0 )  s = bad_data_double;

return ( s );

}


////////////////////////////////////////////////////////////////////////


template <typename T>

T CRC_Array<T>::min() const

{

if ( Nelements == 0 )  return ( bad_data_int );

int j;

T min_v = e[0];

for(j=0; j<Nelements; j++) {

   if ( is_bad_data(e[j]) ) continue;

   if ( e[j] < min_v )  min_v = e[j];

}

return ( min_v );

}


////////////////////////////////////////////////////////////////////////


template <typename T>

T CRC_Array<T>::max() const

{

if(Nelements == 0) return(bad_data_int);

int j;

T max_v = e[0];

for(j=0; j<Nelements; j++) {

   if ( is_bad_data(e[j]) )  continue;

   if ( e[j] > max_v )  max_v = e[j];

}

return ( max_v );

}


////////////////////////////////////////////////////////////////////////


template <typename T>

void CRC_Array<T>::increment(const T & k)

{

int j;

for (j=0; j<Nelements; ++j)  {

   e[j] += k;

}

return;

}


////////////////////////////////////////////////////////////////////////


#endif   /*  __CONTIGUOUS_RETURNS_COPY_ARRAY_H__  */


////////////////////////////////////////////////////////////////////////


