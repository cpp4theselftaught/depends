# depends
The Depends library and its documentation serve two purposes: education and
productivity. I like mixing these two purposes whenever I can, because I
believe it to be very important to continue learning to be productive, and to
be productive to be successful in software development.

##The educational purpose of Depends

Much of Depends is its documentation: it explains in much detail how the
dependency tracker is implemented and should be clear enough for any programmer
to understand both how it is implemented and how to implement one themselves.

##The productivity purpose of Depends

The Depends class implements a complete dependency tracker and an associative
container for any given value-type and can be used as-is without any real
knowledge of how it is implemented. From a productivity point of view, the
extra documentation may be a plus, but is certainly not necessary.

##When was Depends created?

The very first version of Depends was created for the Jail-Ust project, which
is now defunct. The dependency tracker, however, took on a life of its own and 
was improved, debugged, and eventually used for a few production applications
I am aware of. Part of its life was spent in my now-defunct "petri-dish": the
Developer's Corner. It finally re-appeared on SourceForge as its own project
on Aug. 8th 2007, where it was left alone for a long time and where the code
was never hosted. Code was hosted by my company, Vlinder Software, until late
2013, when the public Git server was (silently) shut down. Vlinder Software
continued to maintain a commercial version until mid-2015, when it announced
it would cease commercial support -- and when I decided to put the latest
public version on GitHub.

##Why was Depends created?

For one of the experiments I did on Jail, not all of which are public, I
needed a dependency tracker to track the dependencies between singleton
instances. The Depends library was first created for this sole and unique
purpose. It has since been modified to follow new coding standards and to be
incorporated in a proprietary package version manager, a few interpreters,
a build system, and perhaps a few other applications I am not aware of. 

A draft of an article about dependency tracking has become the documentation.

