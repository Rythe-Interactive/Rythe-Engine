#pragma once
namespace legion::core::interfaces
{

    /** @brief this signal signifies that there is a frontend and a backend function with the same name
     *  - when you want to implement the backend interface override the function with implement_signal_t
     *    as it's first parameter
     *  - when you want to use the frontend interface call the function without implement_signal_t
     *
     * for instance
     * @code 
     *  struct interface {
     *      void erase() const noexcept;
     *      virtual void erase(implement_signal_t) const noexcept;
     *  };
     *
     *  struct impl : interface {
     *      //implementation
     *      void erase(implement_signal_t) override const noexcept {
     *          //erase stuff
     *      }
     *  };
     *
     *  void stuff()
     *  {
     *      //usage
     *      interface* i = new impl;
     *      i->erase();
     *  }
     *
     * @endcode
     *
     */
    struct implement_signal_t {};


}
