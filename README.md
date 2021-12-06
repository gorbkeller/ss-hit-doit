# SS-HIT-DOIT: "Soft-Save; Hard/Intermittent Transfer; Delete Original If Transferred"
### a simple data backup/transfer management scheme 

## motivation
I want to be able to connect to my external drive and execute a single command to free up space without having to reorganize everything at once manually 

## programs
* ss: "soft-save" is a combination of creating a symlink to a file and storing a persistent reference to it
* hit: "hard-transfer" copies the data of interest to the symlink location(s) referenced 
* doit: "delete original if transferred" iterates over the reference file and deletes the existing files which have had a hard transfer executed

## use
a file: any file (or directory TO-DO)
1. execute "ss" on a file
2. do work on the file
3. execute "ht" to save current state of file to spot of existing symlink
4. run out of storage space on local drive 
5. execute "doit" to 'offload' the data to predetermined location by deleting the original 

## reference scheme
* reference file: '/tmp/ss-hit-doit/ref.csv'
* each line denotes a distinct reference 
```
e.g.: original_file.txt, copy_or_link__original.txt, [link/copy/transfer]
```
* line elements:
    * origin: the path to the data 
    * copy: the path to the symlink or copied data 
    * status: state reflecting whether ss/hit/doit was last executed 


## to-do
1. implement ss
2. implement ht
3. implement doit

