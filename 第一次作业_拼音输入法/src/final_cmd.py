from hmm import predict
from data_prep import load_obj

zi_freq_dict = load_obj('zi_freq_dict')
zi_nextzi_dict = load_obj('zi_nextzi_dict')
pin_zi_dict = load_obj('pin_zi_dict')
total_count = sum(zi_freq_dict.values())

if __name__ == '__main__':
    pinyin = input("\nPlease input pinyin(input 'exit' when over): ")
    while(pinyin != "exit"):
        pinyin_list = str.lower(pinyin).split()
        for pin in pinyin_list:
            assert pin in pin_zi_dict.keys(), '输入的不是拼音'
        prediction = predict(pinyin_list)
        print(prediction)
        pinyin = input("Please input pinyin:(input 'exit' when over) ")


