import pickle
import os
import copy
import time
import chardet


def is_zh(ch):
    if '\u4e00' <= ch <= '\u9fff':
        return True
    else:
        return False


def save_obj(obj, name):
    with open(name + '.pkl', 'wb') as f:
        pickle.dump(obj, f, pickle.HIGHEST_PROTOCOL)


def load_obj(name):
    with open(name + '.pkl', 'rb') as f:
        return pickle.load(f)


if __name__ == '__main__':

    dat = open("..\\拼音汉字表_12710172\\拼音汉字表.txt").readlines()
    pin_zi_dict = {}
    for row in dat:
        pin_list = row.split()
        pin_zi_dict[pin_list[0]] = pin_list[1:]
    # save_obj(pin_zi_dict, 'pin_zi_dict')

    time_start = time.time()

    zi_nextzi_dict = {}
    dirname = "..\\sina_news_gbk"
    files = os.listdir(dirname)
    for j in range(len(files)):
        path = os.path.join(dirname,files[j])
        # if os.path.isfile(path) and files[j].startswith('2016'):
        if os.path.isfile(path):
            if files[j].startswith('category'):
                dat = open(path, encoding='UTF-8').readlines()
            else:
                dat = open(path).readlines()
            for line in dat:
                for i in range(len(line)-1):
                    ch = line[i]
                    next_ch = line[i+1]
                    # 如果该字是中文则记录
                    if is_zh(ch):
                        # 如果dict里已经记录了这个字
                        if ch in zi_nextzi_dict.keys():
                            zi_nextzi_dict[ch]['freq'] += 1
                        else:
                            zi_nextzi_dict[ch] = {'freq':1}

                        if is_zh(next_ch):
                            if next_ch in zi_nextzi_dict[ch].keys():
                                zi_nextzi_dict[ch][next_ch] += 1
                            else:
                                zi_nextzi_dict[ch][next_ch] = 1
    save_obj(zi_nextzi_dict,"zi_nextzi_dict")

    zi_freq_dict = {}
    for zi in zi_nextzi_dict.keys():
        freq = zi_nextzi_dict[zi]['freq']
        zi_freq_dict[zi] = freq
    save_obj(zi_freq_dict,"zi_freq_dict")

    # 删去训练集中没有的字
    for pin in pin_zi_dict.keys():
        for ch in pin_zi_dict[pin]:
            if ch not in zi_freq_dict.keys():
                pin_zi_dict[pin].remove(ch)

    save_obj(pin_zi_dict, 'pin_zi_dict')

    time_end = time.time()
    print("Total time using: " + str(time_end - time_start))


