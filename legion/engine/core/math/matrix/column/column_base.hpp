#pragma once
#include <core/types/primitives.hpp>

#include <core/math/vector/vector_base.hpp>

namespace legion::core::math
{
   template<typename Scalar, size_type SizeH, size_type SizeV, size_type ColIdx>
   struct column
   {
       using scalar = Scalar;
       static constexpr size_type size = SizeV;
       static constexpr size_type size_h = SizeH;
       static constexpr size_type size_v = SizeV;
       static constexpr size_type col_idx = ColIdx;
       using type = column<Scalar, SizeH, SizeV, ColIdx>;
       using conv_type = vector<scalar, size>;

       scalar mx[size_h][size_v];

       RULE_OF_5_CONSTEXPR_NOEXCEPT(column);

       constexpr operator conv_type() const noexcept;

       constexpr column& operator=(const conv_type& other) noexcept;
   };

   template<size_type SizeH, size_type SizeV, size_type ColIdx>
   struct column<bool, SizeH, SizeV, ColIdx>
   {
       using scalar = bool;
       static constexpr size_type size = SizeV;
       static constexpr size_type size_h = SizeH;
       static constexpr size_type size_v = SizeV;
       static constexpr size_type col_idx = ColIdx;
       using type = column<bool, SizeH, SizeV, ColIdx>;
       using conv_type = vector<scalar, size>;

       scalar mx[size_h][size_v];

       RULE_OF_5_CONSTEXPR_NOEXCEPT(column);

       constexpr void set_mask(bitfield8 mask) noexcept;
       constexpr bitfield8 mask() const noexcept;

       constexpr operator conv_type() const noexcept;

       constexpr column& operator=(const conv_type& other) noexcept;
   };

   template<typename Scalar, size_type SizeH, size_type ColIdx>
   struct column<Scalar, SizeH, 1, ColIdx>
   {
       using scalar = Scalar;
       static constexpr size_type size = 1;
       static constexpr size_type size_h = SizeH;
       static constexpr size_type size_v = 1;
       static constexpr size_type col_idx = ColIdx;
       using type = column<Scalar, SizeH, 1, ColIdx>;
       using conv_type = vector<scalar, size>;

       scalar mx[size_h][size_v];

       RULE_OF_5_CONSTEXPR_NOEXCEPT(column);

       constexpr operator scalar() const noexcept;

       constexpr operator conv_type() const noexcept;

       constexpr column& operator=(const conv_type& other) noexcept;

       constexpr column& operator=(scalar value) noexcept;
   };

   template<size_type SizeH, size_type ColIdx>
   struct column<bool, SizeH, 1, ColIdx>
   {
       using scalar = bool;
       static constexpr size_type size = 1;
       static constexpr size_type size_h = SizeH;
       static constexpr size_type size_v = 1;
       static constexpr size_type col_idx = ColIdx;
       using type = column<bool, SizeH, 1, ColIdx>;
       using conv_type = vector<scalar, size>;

       scalar mx[size_h][size_v];

       RULE_OF_5_CONSTEXPR_NOEXCEPT(column);

       constexpr void set_mask(bitfield8 mask) noexcept;
       constexpr bitfield8 mask() const noexcept;

       constexpr operator scalar() const noexcept;

       constexpr operator conv_type() const noexcept;

       constexpr column& operator=(const conv_type& other) noexcept;

       constexpr column& operator=(scalar value) noexcept;
   };
}
