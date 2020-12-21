#pragma once
#include <core/types/types.hpp>       // byte_vec
#include <core/platform/platform.hpp> // L_NODISCARD

#include <string_view>                // std::string_view

#include <Optick/optick.h>

#include "detail/resource_meta.hpp"   //has_to_resource<T,Sig>, has_from_resource<T,Sig>


namespace legion::core::filesystem
{

class Test {

};


	/**@class basic_resource
	 * @brief A handle for a basic resource type from which elements can serialize and deserialize from
	 *        ideal for storing elements loaded from disk.
	 */
	class basic_resource
	{
	public:

		/**@brief Constructor that keeps the container empty.
		 * @param [in] placeholder Must be nullptr, used to signal that the resource should be kept empty.
		 */
		explicit basic_resource(std::nullptr_t placeholder) : m_container{}{}

		/**@brief Constructs a basic resource from a legion::core::byte_vec.
		 * @param [in] v The resource from which the resource is created (copy operation/move).
		 */
		explicit basic_resource(byte_vec v) : m_container(std::move(v)) {
            OPTICK_EVENT();
        }

		/**@brief Constructs a basic resource from a std::string
		 * @param [in] v The resource from which the resource is created (copy-assign operation)
		 */
		explicit basic_resource(const std::string_view& v) : basic_resource(nullptr)
		{
            OPTICK_EVENT();
            m_container.assign(v.begin(), v.end());
		}

		//copy & move operations
		basic_resource(const basic_resource& other) = default;
		basic_resource(basic_resource&& other) noexcept = default;
		basic_resource& operator=(const basic_resource& other) = default;
		basic_resource& operator=(basic_resource&& other) noexcept = default;

		//stl operators

		/**@brief Gets an iterator to the first element of the container.
		 * @return iterator to first element
		 */
		L_NODISCARD auto begin() noexcept
		{
			return m_container.begin();
		}
		
		/**@brief Gets an iterator to the first element of the container.
		 * @return iterator to first element
		 */
		L_NODISCARD auto begin() const noexcept
		{
			return m_container.begin();
		}

		/**@brief Gets an iterator to the last element + 1 of the container.
		 * @return iterator to first element
		 */
		L_NODISCARD auto end() noexcept
		{
			return m_container.end();
		}

		/**@brief Gets an iterator to the last element + 1 of the container.
		 * @return iterator to first element
		 */
		L_NODISCARD auto end() const noexcept
		{
			return m_container.end();
		}

		/**@brief Gets a pointer to the data of the container.
		 * @return byte* to raw data
		 */
		L_NODISCARD auto data() noexcept
		{
			return m_container.data();
		}

		/**@brief Gets a pointer to the data of the container.
		 * @return byte* to raw data
		 */
		L_NODISCARD auto data() const noexcept
		{
			return m_container.data();
		}

		/**@brief Gets the size of the container.
		 * @return size_t to the size of container
		 */
		L_NODISCARD auto size() const noexcept
		{
			return m_container.size();
		}

		/**@brief Checks if the container is empty.
		 * @return bool, true when empty
		 */
		L_NODISCARD auto empty() const noexcept
		{
			return m_container.empty();
		}

        void clear() noexcept
        {
            m_container.clear();
        }

		/**@brief Gets the container element
		 * @return legion::core::byte_vec 
		 */
		L_NODISCARD byte_vec& get() noexcept
		{
			return m_container;
		}
		
		/**@brief Gets the container element.
		 * @return legion::core::byte_vec 
		 */
		L_NODISCARD const byte_vec& get() const noexcept
		{
			return m_container;
		}

		/**@brief String assignment operator.
		 * @param value The string you want to assign to the byte_vec.
		 * @return basic_resource& *this
		 */
		basic_resource& operator=(const std::string_view& value)
		{
			m_container.assign(value.begin(),value.end());
			return *this;
		}
		
		/**@brief String conversion.
		 * @return std::string, The container converted to const char *.
		 */
		L_NODISCARD std::string to_string() const
		{
			const char* const cstr = reinterpret_cast<const char*>(data());
			return std::string(cstr,size());
		}


		/**@brief Generic conversion to T
		 *
		 * @note   Tries to convert a basic resource, for this to work T must implement a static
		 *         method with signature T::from_resource(basic_resource&,const T&)
		 *         and must be, A: trivially constructable or B: constructable via Args&&...
		 *         and must be move or copy assignable.
		 *
		 * @tparam T The class you want to convert to.
		 * @param args Additional construction parameters required to create T.
		 * 
		 *         
		 * @return T Created from this resources data.
		 */
		template <class T,class... Args>
		L_NODISCARD T to(Args&&...args) const;

		/**@brief Generic conversion from T.
		 *
		 * @note  Tries to convert T to a basic resource, for this to work T must implement a static
		 *        method with signature T::to_resource(basic_resource*).
		 *
		 * @param v The T you want to convert into a basic_resource.
		 */
		template <class T>
		void from(const T& v);
		
	private:
		byte_vec m_container;
	};

	#ifndef DOXY_EXCLUDE
	/**@cond INTERNAL
	 * @{
	 */
	
	template<typename T,
			 typename C1 = std::enable_if<detail::has_to_resource<T,void(basic_resource*,const T&)>::value>>
	void to_resource(basic_resource* resource,const T& value)
	{
        OPTICK_EVENT();
        T::to_resource(resource,value);
	}

	template<typename T,
			 typename C1 = std::enable_if<detail::has_to_resource<T,void(basic_resource*,const T&)>::value>>
	basic_resource to_resource(const T& value)
	{
        OPTICK_EVENT();
        basic_resource res(nullptr);
		T::to_resource(&res,value);
		return res;
	}

	template<typename T,
			 typename C1 = std::enable_if<detail::has_from_resource<T,void(T*,const basic_resource&)>::value>>
	void from_resource(T* value, const basic_resource& resource)
	{
        OPTICK_EVENT();
        T::from_resource(value, resource);
	}
	
   template<typename T,
			typename C1 = std::enable_if<detail::has_from_resource<T,void(T*,const basic_resource&)>::value>,
			typename C2 = std::enable_if<std::is_default_constructible<T>::value>,
			typename C3 = std::enable_if<std::is_move_constructible<T>::value>>
	T from_resource(const basic_resource& resource)
	{
       OPTICK_EVENT();
       T value;
		T::from_resource(&value,resource);
		return std::move(value);
	}
	
   template<typename T,
			typename C1 = std::enable_if<detail::has_from_resource<T,void(T*,const basic_resource&)>::value>,
			typename C2 = std::enable_if<std::is_move_constructible<T>::value>,
			class ... Args>
	T from_resource(const basic_resource& resource,Args&&... args)
	{
       OPTICK_EVENT();
       T value(std::forward<Args>(args)...);
		T::from_resource(&value,resource);
		return std::move(value);
	}


	template <class T,class... Args>
	L_NODISCARD T basic_resource::to(Args&&...args) const
	{
        OPTICK_EVENT();
        return std::move(from_resource<T>(*this, std::forward<Args>(args)...));
	}

	template <class T>
	void basic_resource::from(const T& v)
	{
        OPTICK_EVENT();
        to_resource(this,v);
	}
	/**
	 * //cond
	 * @}
	 */

    namespace literals
	{
        /**@brief. creates a view from a string literal
         */
       inline basic_resource operator""_res(const char* str,size_type len)
       {
           return basic_resource(std::string_view(str,len));
       }

	}

	#endif
}
