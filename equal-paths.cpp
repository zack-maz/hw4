#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here


bool equalPaths(Node * root)
{
    if (root == nullptr) return true;
    int leftHeight = getHeight(root->left);
    int rightHeight = getHeight(root->right);
    if (leftHeight == 1 && rightHeight == 0) return true;
    if (leftHeight == 0 && rightHeight == 1) return true;
    return leftHeight == rightHeight;

}

int getHeight(Node* root)
{
    if (root == nullptr) return 0;
    int l = getHeight(root->left);
    int r = getHeight(root->right);

    if (l > r) return l+1;
    else return r+1;
}
