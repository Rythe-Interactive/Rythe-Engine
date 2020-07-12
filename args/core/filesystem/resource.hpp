#pragma once
#include <core/types/types.hpp>       // byte_vec
#include <core/platform/platform.hpp> // A_NODISCARD

#include <string_view>                // std::string_view

#include "detail/resource_sfinae.hpp" //has_to_resource<T,Sig>, has_from_resource<T,Sig>


/**
 * @file resource.hpp
 */

/**
 * @addtogroup args_fs
 * @{
 */
namespace args::core::filesystem
{
	
	class basic_resource
	{
	public:

		/**@brief constructor that keeps the container empty
		 * @param [in] 1 must be nullptr, used to signal that the resource should be kept empty
		 */
		explicit basic_resource(nullptr_t) : m_container{}{}

		/**@brief constructs a basic resource from a args::core::byte_vec
		 * @param [in] v the resource from which the resource is created (copy operation/move)
		 */
		explicit basic_resource(byte_vec v) : m_container(std::move(v)) {}

		/**@brief constructs a basic resource from a std::string
		 * @param [in] v the resource from which the resource is created (copy-assign operation)
		 */
		explicit basic_resource(const std::string_view& v) : basic_resource(nullptr)
		{
			m_container.assign(v.begin(), v.end());
		}

		//copy & move operations
		basic_resource(const basic_resource& other) = default;
		basic_resource(basic_resource&& other) noexcept = default;
		basic_resource& operator=(const basic_resource& other) = default;
		basic_resource& operator=(basic_resource&& other) noexcept = default;

		//stl operators

		/**@brief gets an iterator to the first element of the container
		 * @return iterator to first element
		 */
		A_NODISCARD auto begin() noexcept
		{
			return m_container.begin();
		}
		
		/**@brief gets an iterator to the first element of the container
		 * @return iterator to first element
		 */
		A_NODISCARD auto begin() const noexcept
		{
			return m_container.begin();
		}

		/**@brief gets an iterator to the last element + 1 of the container
		 * @return iterator to first element
		 */
		A_NODISCARD auto end() noexcept
		{
			return m_container.end();
		}

		/**@brief gets an iterator to the last element + 1 of the container
		 * @return iterator to first element
		 */
		A_NODISCARD auto end() const noexcept
		{
			return m_container.end();
		}

		/**@brief gets a pointer to the data of the container
		 * @return byte* to raw data
		 */
		A_NODISCARD auto data() noexcept
		{
			return m_container.data();
		}

		/**@brief gets a pointer to the data of the container
		 * @return byte* to raw data
		 */
		A_NODISCARD auto data() const noexcept
		{
			return m_container.data();
		}

		/**@brief gets the size of the container
		 * @return size_t to the size of container
		 */
		A_NODISCARD auto size() const noexcept
		{
			return m_container.size();
		}

		/**@brief checks if the container is empty
		 * @return bool, true when empty
		 */
		A_NODISCARD auto empty() const noexcept
		{
			return m_container.empty();
		}

		/**@brief gets the container element
		 * @return args::core::byte_vec 
		 */
		A_NODISCARD byte_vec& get() noexcept
		{
			return m_container;
		}
		
		/**@brief gets the container element
		 * @return args::core::byte_vec 
		 */
		A_NODISCARD const byte_vec& get() const noexcept
		{
			return m_container;
		}

		/**@brief string assignment operator
		 * @param value the string you want to assign to the byte_vec
		 * @return basic_resource& *this
		 */
		basic_resource& operator=(const std::string_view& value)
		{
			m_container.assign(value.begin(),value.end());
			return *this;
		}
		
		/**@brief string conversion
		 * @return std::string, the container converted to const char *
		 */
		A_NODISCARD std::string to_string() const
		{
			const char* const cstr = reinterpret_cast<const char*>(data());
			return std::string(cstr,size());
		}


		/**@brief generic conversion to T
		 *
		 * tries to convert a basic resource, for this to work T must implement a static
		 *         method with signature T::from_resource(basic_resource&,const T&)
		 *         and must be, A: trivially constructable or constructable via Args&&...
		 *         and must be move or copy assignable
		 *
		 * @tparam T the class you want to convert to
		 * @param args additional construction parameters required to create T
		 * 
		 *         
		 * @return T created from this resources data
		 */
		template <class T,class... Args>
		A_NODISCARD T to(Args&&...args);

		/**@brief generic conversion from T
		 *
		 * tries to convert T to a basic resource, for this to work T must implement a static
		 *        method with signature T::to_resource(basic_resource*)
		 *
		 * @param v the T you want to convert into a basic_resource
		 */
		template <class T>
		void from(const T& v);
		
	private:
		byte_vec m_container;
	};


	/**@cond INTERNAL
	 * @{
	 */
	
	template <	typename T,
				typename = std::enable_if<detail::has_to_resource<T,void(basic_resource*,const T&)>::value>>
	void to_resource(basic_resource* resource,const T& value)
	{
		T::to_resource(resource,value);
	}

	template <	typename T,
				typename = std::enable_if<detail::has_to_resource<T,void(basic_resource*,const T&)>::value>>
	basic_resource to_resource(const T& value)
	{
		basic_resource res(nullptr);
		T::to_resource(&res,value);
		return res;
	}
	

	template <	typename T,
				typename = std::enable_if<detail::has_from_resource<T,void(T*,const basic_resource&)>::value>>
	void from_resource(T* value, const basic_resource& resource)
	{
		T::from_resource(value,resource);
	}
	template <	typename T,
				typename = std::enable_if<detail::has_from_resource<T,void(T*,const basic_resource&)>::value>,
				typename = std::enable_if<std::is_default_constructible<T>::value>,
				typename = std::enable_if<std::is_move_constructible<T>::value>>
	T from_resource(const basic_resource& resource)
	{
		T value;
		T::from_resource(&value,resource);
		return std::move(value);
	}
		template <	typename T,
				typename = std::enable_if<detail::has_from_resource<T,void(T*,const basic_resource&)>::value>,
				typename = std::enable_if<std::is_move_constructible<T>::value>,
				class ... Args>
	T from_resource(const basic_resource& resource,Args&&... args)
	{
		T value(std::forward<Args>(args)...);
		T::from_resource(&value,resource);
		return std::move(value);
	}


	template <class T,class... Args>
	A_NODISCARD T basic_resource::to(Args&&...args)
	{
		return std::move(from_resource(*this,std::forward<Args>(args)...));
	}

	template <class T>
	void basic_resource::from(const T& v)
	{
		to_resource(this,v);
	}
}

/**
 * //cond
 * @}
 * //addtogroup
 * @} 
 */