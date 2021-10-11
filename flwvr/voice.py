from selenium import webdriver
from selenium.webdriver.support.ui import Select

import time


#good_voices = [0, 1, 2, 5, 6, 7, 8, 10, 11, 12, 13, 21, 22, 23, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 43, 45, 46, 47, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 61]
good_voices = [0, 31, 12, 9, 8]
def start_webdriver():
    chrome_options = webdriver.ChromeOptions()
    prefs = {"download.default_directory" : "C:\\Users\\Jordan\\Test-Folder\\FLWG\\flwvr\\subset_samples\\" }
    chrome_options.add_experimental_option("prefs", prefs)
    driver = webdriver.Chrome(executable_path='C:\\Users\\Jordan\\Test-Folder\\FLWG\\flwvr\\chromedriver', options=chrome_options)
    return driver



driver = start_webdriver()

driver.get('https://ttsmp3.com/')

textbox = driver.find_element_by_xpath('//div/form/textarea[@id="voicetext"]')
accentSelect = Select(driver.find_element_by_xpath('//div/form/select[@id="sprachwahl"]'))
download = driver.find_element_by_xpath('//div/form/input[@id="downloadenbutton"]')
read_button = driver.find_element_by_xpath('//div/form/input[@id="vorlesenbutton"]')

all_words_path = "C:\\Users\\Jordan\\Test-Folder\\FLWG\\docs\\t\\subset.txt"
f = open(all_words_path, "r")
file_text = f.read()
word_list = file_text.split("\n")


def download_word(word):
    # Inserts the word
    textbox.send_keys(word)
    # Changes the accent and downloads the word
    for i in good_voices:
        accentSelect.select_by_index(i)
        driver.execute_script("arguments[0].click();", read_button)
        time.sleep(1)
        driver.execute_script("arguments[0].click();", read_button)
        driver.execute_script("arguments[0].click();", download)
        
        
    # Erases the word
    for i in range(4):
        textbox.send_keys('\b')


for word in word_list:
    download_word(word)

driver.quit()

    
    
    







    


