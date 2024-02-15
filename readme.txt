Hi all, putting comments and questions here:

2/14 and 2/15 edits:

- cleaned up code
- added rand
- added strideN
- tested it out, we're getting outputs from each check




edits after meeting w/ Joe:
- commented out consump tensors
- try using rand w/ C library   
    - then try poplar
    - if nothing works try sttramin in random numbers from cpu


2/8 comments:

- read-first:
    - For Joe: mainly the questions listed in the lines below
    - For Danton: the questions in the codelet for us to figure out

- edits:
    - everything is still named firehose for now until we're able to test the code I guess
    - pseudocode written out adapted from the codelet from firehose--how would one fit
    multiple functions into one codelet
    - there are a buncha questions in the codelet about the algorithms themselves
    - going to list the question locations for firehose_ipu.cpp here:
    - line 96
    - line 105
    - line 109
    - line 111
    - line 115
    - line 131
    - line 146
    - line 162
    - line 181
    - line 181
    - line 205
    - line 209
    - line 219

- to-do:
    - remove second output tensor from firehose_ipu.cpp
    - rename files from firehose to circustent
