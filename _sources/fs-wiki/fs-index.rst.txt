Legion-FS
=========

Legion-FS (also known as pixl-fs) is the subordinate core module for all things file access. It provides a flexible, extensible and easy way to work
with files.

The Basics
----------

To read or write a file you need to create a view for that file


.. code-block:: cpp
    :linenos:


    //...
    namespace fs = legion::core::filesystem;
    test = fs::view("assets://examples/test.txt");


There is a few interesting things going on here:

- First notice how we renamed the namespace from ``legion::core::filesystem`` to ``fs`` this is mostly for brevity and I would recommend to use the filesystem 
  in this way, as file operations are rather common!
- Second you can see that we are working with a weird file-path here. You probably do not have an ``assets:\\`` drive on your system. Even if you did, this would not map to that.
  Assets is what is called a `Virtual provider <virtual_filesystems.rst>`_. We get into what that means later, for now all you need to know is that it maps to 
  ``<your-project-directory>\assets\`` 


Next let's attempt to write to this file


.. code-block:: cpp
    :linenos:


    fs::basic_resource resource("Hello World");
    test.set(resource);


Here we can see the introduction of a ``basic_resource`` we are going to discuss what these are used for in `Serialization <automatic_conversion.rst>`_ later.
For now we just need one of its basic constructors that creates a resource from a string. Using ``set()`` we can write this resource to disk at the provided location.
You can also do this in one line:


.. code-block:: cpp


    fs::view("assets://example/test.txt").set(fs::basic_resource("Hello World"));


Since this is still a lot to write there is also an alternative syntax available:


.. code-block:: cpp

    //this is required once only
    using namespace fs::literals;

    "assets://examples/test.txt"_view.set("Hello World"_res);


if you have never used string literal-syntax before this might look quite alien to you. You do not have to use this, it is just a shorthand.
For more information on user defined literals visit `cppreference <https://en.cppreference.com/w/cpp/language/user_literal>`_

Now let us read back that file:

.. code-block:: cpp

    
    auto contents = test.get().except([](auto err) {
        log::warn("Could not read file \"test.txt\": {}",err.what());
        return basic_resource("error");
    }

    log::debug("Contents of file: {}", contents.to_string())


Woah, what happened here ? what is all this weird syntax ?
Let's go through this step by step!

- We reuse the good old `test` view here, of course you could create a new one instead, but this will work just fine.
- Then we call the ``get()`` method of the view. Since reading a file might not succeed we have to do something about this.
- This is where ``.except()`` comes into play, it is a function of ``result``, you can read more about results 
  ` here <../architecture-wiki/result.rst>`_. A result needs to be resolved, the safest way to do this is using either `except`
  or manually check if there is an error with `has_err()`. Here we check with ``except`` and return a different value if there was 
  an error.

- Finally we log the contents of the file, note that we use ``.to_string()`` to turn the ``basic_resouce`` back into a std::string.

.. toctree::
    :maxdepth: 2

    virtual_filesystems.rst
    automatic_conversion.rst
    asset_import_pipelines.rst
    advanced_filesystem_usage.rst
    custom_providers.rst
    