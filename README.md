```bash
1、初始化一个仓库
git init

2、工作区、暂存库和版本库
git add <n个文件>  将工作区中的文件添加到暂存库
git commit -m "注释" 将暂存库中的提交到版本库

3、日志和状态信息
git log
git status
git diff  工作区和暂存库的区别

4、删除文件
git rm <n个文件>  删除工作区中的文件然后提交到暂存库

5、版本回退
git reset --hard  移除工作区和暂存库中的修改，回退到版本库最近一次的提交，此时全部一致。
git reset   移除暂存库的修改，回退到版本库最近一次的提交，工作区不做变动

6、撤销修改
git restore <文件>  丢弃工作区的修改，仅仅针对文件内容
git checkout -- <文件> 丢弃工作区的修改，并用最近一次的git add状态进行替换。仅仅针对文件内容

7、上传与下载
git pull origin main   从github仓库中拉取数据
git pull origin main     将本地版本库中的数据推送到github网站仓库中

git push -u origin "main"  将本地仓库数据上传，适用于创建了空仓库，通过本地仓库上传数据
origin 是git remote add origin git@github.com::xxx.git设置的一个远程仓库地址的别名
main是git branch -M "main" 设置的一个分支名

```

