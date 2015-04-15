Basic Git Usage/Tips
==

Basic branching
--
* `git branch` will list all the branches you have on your machine
* `git checkout [branch-name]` will allow you to switch to that branch, from there, any changes commited will be on that branch.
* **Important:** double check that you are on the correct branch before making changes, there's ways to fix it if you accidentally work on the wrong branch though.
* `git push origin [branch-name]` will publish changes you've made to github, the changes will still be associated with that branch.
* **Iportant:** commit working states often, and push changes cautiously.

Branch Naming conventions
--
* The name of the branch should describe its purpose
* usually it begins with `feature-....` or `bugfix-...`, but it can begin with whatever really.

To create a new branch on your machine
--
* `git checkout -b [new-branch-name]` will create the branch and put you on it.

To pull branch off github that is not on your machine
--
* This will either copy the branch to your machine if you do not have it, or update your branch with current changes if you do have it.
* (if the branch exists on your machine already) `git checkout [this-branch-name]`
* then, `git pull` might work depending upon your settings, but `git pull origin [this-branch-name]` will always work.
* (if the branch does not exist on your machine) `git pull origin [this-branch-name]`

Making sure your branch will integrate with development: It's a good idea to do this every so often.
--
* Make sure you have your branch clean (there are no changes hanging around that you haven't commited yet)
* checkout development: `git checkout development`
* get updates that may have been publushed to development: `git pull origin development`
* return to your branch: `git checkout [your-branch-name]`
* merge **development** into **your branch**: `git merge development` (there may be conflicts you need to fix, I'll work on helping you understand that as it comes up
* Run the program to verify that your branch is compatible with development, and will work smoothly with the code.

When you're finished with your branch and ready to integrate it with development
--
* Make sure your branch will integrate with development by following the above proceedure.
* Commit that you've merged development into your branch `git commit -m "merged development, fixes #33"
* note: if you tag on "fixes #30" or "closes #30" in the commit message, when you push your changes, github will automatically close the corresponding issue you were working on. (there are more phrases, but those work)
* `git push origin [branch-name]`
* Create a pull request: go to pull requests on github, and submit one for your branch.
* someone verifies that you're code functions correctly, and then they merge it to development.
