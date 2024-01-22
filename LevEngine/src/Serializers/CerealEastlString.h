#pragma once
#include "cereal/cereal.hpp"
#include <EASTL/string.h>

namespace cereal
{
    //! Serialization for basic_string types, if binary data is supported
    template<class Archive, class CharT> inline
    typename eastl::enable_if<traits::is_output_serializable<BinaryData<CharT>, Archive>::value, void>::type
    CEREAL_SAVE_FUNCTION_NAME(Archive & ar, eastl::basic_string<CharT> const & str)
    {
        // Save number of chars + the data
        ar( make_size_tag( static_cast<size_type>(str.size()) ) );
        ar( binary_data( str.data(), str.size() * sizeof(CharT) ) );
    }

    //! Serialization for basic_string types, if binary data is supported
    template<class Archive, class CharT> inline
    typename eastl::enable_if<traits::is_input_serializable<BinaryData<CharT>, Archive>::value, void>::type
    CEREAL_LOAD_FUNCTION_NAME(Archive & ar, eastl::basic_string<CharT> & str)
    {
        size_type size;
        ar( make_size_tag( size ) );
        str.resize(size);
        ar( binary_data( const_cast<CharT *>( str.data() ), size * sizeof(CharT) ) );
    }
} // namespace cereal

