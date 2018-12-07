//#include "type.h"

int my_link(char *file1, char *file2) {
    //var def
    int ino, p_ino, test_ino;
    MINODE *mip = running->cwd, *p_mip;
    INODE *inode, *p_inode;
    char new_path[128], to_create[128];
    char new_file[128], old_file[128], temp[128];

    printf("in my_link\n");
    
    //putting files into buffers to be altered
    strcpy(new_file, file2);
    strcpy(old_file, file1);

    //makign sure both parameters are specified
    if (!strcmp(old_file, "") || !strcmp(new_file, "")) {
        printf("error: not enough files specified: exiting\n");
    }
    //starting here with the basic checks
    //first, get the inode
    ino = getino(mip, old_file);
    printf("ino: %d\n", ino);

    //check the ino to make sure the file exists
    if(ino == 0) {
        printf("error: path for old_file doesn't exist: exiting\n");
        return 0;
    }

    //set mip to the first file MINODE
    mip = iget(dev, ino);
    printf("mip set\n");

    //make sure the file is a file, not a dir
    if (S_ISDIR(mip->INODE.i_mode)) {
        printf("error: cannot make a link to a dir: exiting\n");
                return 0;
    }
    printf("src file is not a dir\n");

    //now that the old file is set up, time to find the place 
    //the new file
    printf("old_file: %s\tnew_file: %s\n", old_file, new_file);
    if (!strcmp(new_file, "/")) {
        //dir specified is just the root
        strcpy(new_path, "/");
    }
    else {
        //root not specified, so get the dirname from new_file
        strcpy(temp, new_file);
        strcpy(new_path, dirname(temp));
    }
    printf("new_path dir_path saved\n");
    //now the dirname is saved, check that the basename doesn
    //already exist
    strcpy(temp, new_file);
    printf("temp: %s\n", temp);
    printf("%s\n", basename(temp));
    strcpy(temp, new_file);
    strcpy(to_create, basename(temp));

    printf("to_create: %s\n", to_create);
    //now that we have the split path, time to make sure dirname
    //exists and basename doesn't
    //get the ino of the directory
    p_ino = getino(running->cwd, new_path);
    
    //get the MINODE of the dir
    p_mip = iget(dev, p_ino);

    //get inode of new MINODE
    p_inode = &p_mip->INODE;

    //make sure the dir exists
    if (!p_mip) {
        printf("error: directory to create file in doesn't exist: exiting\n");
        return 0;
    }

    //now make sure it not only exists but also is a dir
    if (!S_ISDIR(p_inode->i_mode)) {
        printf("error: cannot create a file in a non-dir: exiting\n");
        return 0;
    }
    printf("parent is a dir\n");        
    //set test ino to see if the child exists
    test_ino = getino(running->cwd, new_file);

    //now make sure that the child doesn't exist
    if (test_ino) {
        printf("error: file being created already exists: exiting\n");
        return 0;
    }

    //now all of the testing is done, so we can move on to 
    //making the new file!
    //use the ino of the old file, and use it to make a file
    //with that matching ino and given basename
    printf("enter name:\n");
    strcpy(temp, new_file);
    enter_name(p_mip, ino, basename(temp));

    //set the pointer inode to the original INODE
    inode = &mip->INODE;

    //new link has been created, so update the link count
    inode->i_links_count = inode->i_links_count++;

    //finish up by marking dirty
    mip->dirty = 1;
    
    p_inode = &p_mip->INODE;

    p_inode->i_atime = time(0L);

    p_mip->dirty = 1;

    //put back both of the mip's
    iput(p_mip);
    iput(mip);

    return 0;
}