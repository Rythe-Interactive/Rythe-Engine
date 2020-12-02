Virtual Filesystems
===================

In Legion, file operations are abstracted away behind what we call **Virtual Filesystems**.
These Filesystems provide a way to interact with a users pc that is both safe and convenient.
Let us look at an example:

.. code-block:: cpp


    // we want to access a file in a folder on the users pc
    // we first create a virtual provider pointing to the folder
    fs::provider_registry::domain_create_resolver<filesystem::basic_resolver>("folder://", "./folder");

    //then we can access the files in that folder
    auto result = fs::view("folder://file.txt").get();

This allows us to work with the filesystem in little bubbles that can be moved without having to fix a lot of code
For instance. imagine one day you decide to move the `folder` into a different location, let's say alongside your project
Instead of having to fix every file access all you need to do is

.. code-block:: cpp


    // we want to access a file in a folder on the users pc
    // we first create a virtual provider pointing to the folder
    fs::provider_registry::domain_create_resolver<filesystem::basic_resolver>("folder://", "../folder");
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^^
    //Notice how we changed the location of the resolver to ../folder instead of ./folder


    //then we can access the files in that folder
    auto result = fs::view("folder://file.txt").get();


They also provide a little bit of security. 
Let's say your game supports mods. You do not want those mods to access files outside of their own little sandbox.

.. code-block:: cpp


    // we create a new virtual filesystem for the mod
    fs::provider_registry::domain_create_resolver<filesystem::basic_resolver>("my_cool_mod://", "./mods/my_cool_mod");

    //then we pass the view to the mod
    mods["my_cool_mod"].initialize(fs::view("my_cool_mod://"))


If no other method of accessing files other than this view exists, then the mod is locked into this sandbox
trying to access files before this folder will fail!

.. code-block:: cpp


    //we try to be sneaky and access something we shouldn't

    void mod_init(fs::view view)
    {
        view["../../folder/test.txt"].get() // FAIL! this will not access the file the malicious actor wanted
                                            // instead he is going to get mods/my_cool_mod/folder/test.txt instead
    }

