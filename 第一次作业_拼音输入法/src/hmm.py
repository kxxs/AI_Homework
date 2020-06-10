from data_prep import save_obj, load_obj
import numpy as np
import copy
import re

zi_freq_dict = load_obj('zi_freq_dict')
zi_nextzi_dict = load_obj('zi_nextzi_dict')
pin_zi_dict = load_obj('pin_zi_dict')
# pin_zi_dict_v2 = load_obj('pin_zi_dict_v2')
# pin_zi_dict = pin_zi_dict_v2
total_count = sum(zi_freq_dict.values())


def predict(pin_list, cnt_total=total_count, lambd=0.9):
    hmm_zi_dict = {}
    hmm_v_dict = {}
    hmm_argpref_dict = {}
    # pin_list = pin_str.split()
    pin_len = len(pin_list)
    if(pin_len < 1):
        return ''

    candidates = pin_zi_dict[pin_list[0]]
    hmm_zi_dict[0] = candidates
    hmm_v_dict[0] = np.zeros((len(candidates), 1))
    for i in range(len(candidates)):
        hmm_v_dict[0][i] = zi_freq_dict[candidates[i]] / cnt_total

    for t in range(1, pin_len):
        last_zi_list = hmm_zi_dict[t-1]
        current_zi_list = pin_zi_dict[pin_list[t]]
        hmm_zi_dict[t] = current_zi_list
        hmm_v_dict[t] = np.zeros((len(current_zi_list), 1))
        hmm_argpref_dict[t] = np.zeros((len(current_zi_list), 1))

        # pij_mat = np.zeros((len(last_zi_list), len(current_zi_list)))
        # print(current_zi_list)
        for j in range(len(current_zi_list)):
            pij_array = np.zeros((len(last_zi_list), 1))
            vij_array = np.zeros((len(last_zi_list), 1))
            for i in range(len(last_zi_list)):
                last_zi = last_zi_list[i]
                current_zi = current_zi_list[j]
                cnt_i = zi_freq_dict[last_zi]
                cnt_j = zi_freq_dict[current_zi]
                if current_zi in zi_nextzi_dict[last_zi].keys():
                    cnt_ij = zi_nextzi_dict[last_zi][current_zi]
                else:
                    cnt_ij = 0

                pij_array[i] = lambd*cnt_ij/cnt_i + (1-lambd)*cnt_j/cnt_total
                vij_array[i] = hmm_v_dict[t-1][i] * pij_array[i]
            hmm_v_dict[t][j] = np.max(vij_array)
            hmm_argpref_dict[t][j] = np.argmax(vij_array)

    final_str_list = [0]*pin_len
    t = pin_len-1
    zi_arg = np.argmax(hmm_v_dict[t])
    final_str_list[pin_len-1] = hmm_zi_dict[t][zi_arg]
    if pin_len == 1:
        return hmm_zi_dict[t][zi_arg]
    last_zi_arg = int(hmm_argpref_dict[t][zi_arg][0])
    for t in range(pin_len-2,0,-1):
        zi_arg = last_zi_arg
        final_str_list[t] = hmm_zi_dict[t][zi_arg]
        last_zi_arg = int(hmm_argpref_dict[t][zi_arg][0])
    zi_arg = last_zi_arg
    final_str_list[0] = hmm_zi_dict[0][zi_arg]

    return ''.join(ch for ch in final_str_list) + '\n'


if __name__ == '__main__':
    # dat = open("..\\拼音汉字表_12710172\\拼音输入法测试样例.txt",'r',encoding='UTF-8').readlines()
    dat = open("拼音输入法测试样例.txt",'r',encoding='UTF-8').readlines()
    inputs = [dat[i] for i in range(0, len(dat), 2)]
    outputs_real = [dat[i] for i in range(1, len(dat), 2)]
    outputs_predict = []

    zi_correct_num = 0
    zi_num = 0
    ju_correct_num = 0
    ju_num = 0
    for i in range(len(outputs_real)):
        pinyin = str.lower(inputs[i]).split()
        prediction = predict(pinyin, lambd=0.7)
        outputs_predict.append(prediction)
        
        tmp_real = re.sub("\n", "", outputs_real[i])
        tmp_predict = re.sub("\n", "", prediction)

        if len(tmp_real) < 1 or len(tmp_real) != len(pinyin):
            continue
        assert len(tmp_real) == len(tmp_predict), 'prediction length error'

        sentence_correct = True
        for j in range(len(tmp_real)):
            if tmp_real[j] == tmp_predict[j]:
                zi_correct_num += 1
            else:
                sentence_correct = False
            zi_num += 1
        ju_num += 1
        if sentence_correct:
            ju_correct_num += 1
    print('字数：'+str(zi_num))
    print('字正确率：{:.4f}'.format(zi_correct_num/zi_num))
    print('句数：'+str(ju_num))
    print('句正确率：{:.4f}'.format(ju_correct_num/ju_num))

    f_output = []
    for i in range(len(outputs_real)):
        f_output.append(inputs[i])
        f_output.append(outputs_real[i])
        f_output.append(outputs_predict[i])

    f = open("测试结果.txt", "w")
    f.write("".join(f_output))
    f.close()
    '''
    pin_str = str.lower('wo hao ju a').split()

    print(pin_str)
    print(predict(pin_str))
    '''













