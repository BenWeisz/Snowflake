# libSnowflake

## Purpose & Goals

libSnowflake provides you with a program that takes your static data layout as an input and generates specific reader / writer code for that data layout. These layouts often change during developement and so layout specific parsers can be very fragile, just like Snowflakes ❄️

I really really really dislike using file parsing libraries that do the following:

- Ask you to use functions to query a data tree having you handle exception handling
- Being bloated with a much of extra files in the github repo and involving a complex integration into your project
- Using a bunch of dependecies that you have to download

To address this libSnowflake takes a two part approach:

- Provide you with code for a program that will generate a reader / writer for your custom data layout in your custom config file
- Does the above with as few files as possible, with a very easy integration procedure for your projects.
