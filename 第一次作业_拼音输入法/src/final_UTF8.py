from hmm import predict
from data_prep import load_obj
import sys

zi_freq_dict = load_obj('zi_freq_dict')
zi_nextzi_dict = load_obj('zi_nextzi_dict')
pin_zi_dict = load_obj('pin_zi_dict')
total_count = sum(zi_freq_dict.values())

if __name__ == '__main__':
    input_path = "..\\data\\input.txt"
    output_path = "..\\data\\output.txt"
    if len(sys.argv) > 1:
        input_path = sys.argv[1]
    if len(sys.argv) > 2:
        output_path = sys.argv[2]

    try:
        inputs = open(input_path, 'r', encoding='UTF-8').readlines()
    except IOError:
        print('没找到指定路径文件(默认是..//data//input.txt), 或文件编码不是UTF-8')
    else:
        print('成功读入'+input_path)

    output = []
    for i in range(len(inputs)):
        pinyin = str.lower(inputs[i]).split()
        prediction = predict(pinyin)
        output.append(prediction)

    f = open(output_path, "w")
    f.write("".join(output))
    f.close()
    print('成功写入'+output_path)

