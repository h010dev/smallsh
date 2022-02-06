//
// Created by mohamed on 2/3/22.
//

//https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html#tag_18_08_02
void status(int status)
{
        switch (status) {
                case 127:
                        // command not found
                        break;
                case 126:
                        // command not executable
                        break;
                default:
                        break;
        }
}

// global file status, set_status, get_status ?
