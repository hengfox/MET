

////////////////////////////////////////////////////////////////////////


#ifndef  __MTD_3D_ATT_PAIR_ARRAY_H__
#define  __MTD_3D_ATT_PAIR_ARRAY_H__


////////////////////////////////////////////////////////////////////////


#include <iostream>

#include "3d_att.h"


////////////////////////////////////////////////////////////////////////


class PairAtt3DArray {

   private:

      void init_from_scratch();

      void assign(const PairAtt3DArray &);

      void extend(int);


      int Nelements;

      int Nalloc;

      int AllocInc;

      PairAtt3D * e;


   public:

      PairAtt3DArray();
     ~PairAtt3DArray();
      PairAtt3DArray(const PairAtt3DArray &);
      PairAtt3DArray & operator=(const PairAtt3DArray &);

      void clear();

      void dump(ostream &, int = 0) const;

         //
         //  set stuff
         //


         //
         //  get stuff
         //

      int n_elements() const;

      int n         () const;

      PairAtt3D & operator[](int) const;

      int fcst_obj_number(int index) const;  //  one-based
      int  obs_obj_number(int index) const;  //  one-based

      double total_interest(int index) const;

         //
         //  do stuff
         //

      void set_alloc_inc(int = 0);   //  0 means default value (100)

      void add(const PairAtt3D &);
      void add(const PairAtt3DArray &);


};


////////////////////////////////////////////////////////////////////////


inline int PairAtt3DArray::n_elements() const { return ( Nelements ); }
inline int PairAtt3DArray::n         () const { return ( Nelements ); }


////////////////////////////////////////////////////////////////////////


#endif   /*  __MTD_3D_ATT_PAIR_ARRAY_H__  */


////////////////////////////////////////////////////////////////////////


