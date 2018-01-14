# C Data Structures and Algorithms
This repository is an ongoing project of implementing generic data structures and algorithms in ANSI C. I find I often use the same constructs that require a lot of boilerplate code, so I made this repository to both organize these constructs and allow easy integration of these constructs into existing projects.

My design philosophy with this repository is to offer a minimalistic API for each data structure, containing all tools needed to easily build more complex/niche constructs. Portability is top priority, so only ANSI C is used, and no header/source pair relies on another header/source pair. Furthermore, to promote being used in embedded systems, iterative algorithms are used exclusively over recursive algorithms, and each data structure is intrusive (for better memory locality).

Feel free to use these data structures and algorithms in your own way. The code is licensed under the ISC license (a simplified version of the BSD license that is functionally identical); thus, it may legitimately be reused in any project, whether Proprietary or Open Source.

