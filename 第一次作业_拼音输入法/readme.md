# 拼音输入法作业 

电62班 李明轩 2015010705



src文件夹下以下三个文件用于实验验收：

1. **final_cmd.py:** 用于在命令行中手动输入拼音，每输入一行拼音，按回车显示预测结果
2. **final_UTF8.py**:  用于**UTF8格式**的txt文件的输入输出，默认输入路径为"../data/input.txt"，默认输出路径为"../data/output.txt"，**如果要修改路径，则可以在命令行中加入输入输出路径，如：**“**python final_file_UTF8.py ../data0/input.txt ../data0/output.txt“**
3. **final_ANSI.py**: 用于**ANSI格式**的txt文件输入输出，用法同2



src文件夹下其他文件：

1. data_prep.py：根据训练集进行统计，以词典形式输出到pkl文件中，用于在线预测时计算概率
2. hmm.py：主要是一个predict函数，用viterbi算法进行在线序列预测，final_UTF8.py等文件都是直接调用这里的predict函数。