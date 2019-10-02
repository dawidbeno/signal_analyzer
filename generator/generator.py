#! /usr/bin/python

# V pripade Runtime Error na Macos:
# 1. Vytvorit subor 'matplotlibrc' v priecinku ~/.matplotlib
# 2. Do suboru vlozit kod 'backend: TkAgg' a nic viac


import random
from random import randint as r
import pylab
import numpy
import scipy.signal as signal
from random import shuffle
import matplotlib.pyplot as plt
import sys

# use ggplot style for more sophisticated visuals
plt.style.use('ggplot')


def live_plotter(x_vec, y1_data, line1, identifier='', pause_time=0.01):
    if line1 == []:
        # this is the call to matplotlib that allows dynamic plotting
        plt.ion()
        fig = plt.figure(figsize=(10, 6))
        ax = fig.add_subplot(111)
        # create a variable for the line so we can later update it
        line1, = ax.plot(x_vec, y1_data, '-o', alpha=0.8)
        # update plot label/title
        plt.ylabel('Y Label')
        plt.title('Title: {}'.format(identifier))
        plt.show()

    # after the figure, axis, and line are created, we only need to update the y-data
    line1.set_ydata(y1_data)
    # adjust limits if new data goes beyond bounds
    if numpy.min(y1_data) <= line1.axes.get_ylim()[0] or numpy.max(y1_data) >= line1.axes.get_ylim()[1]:
        plt.ylim([numpy.min(y1_data) - numpy.std(y1_data), numpy.max(y1_data) + numpy.std(y1_data)])
    # this pauses the data so the figure/axis can catch up - the amount of pause can be altered above
    plt.pause(pause_time)

    # return line so we can update it again in the next iteration
    return line1

def generate_type_1_wave():
    print('Simulating heart ecg')

    # The "Daubechies" wavelet is a rough approximation to a real,
    # single, heart beat ("pqrst") signal
    pqrst = signal.wavelets.daub(10)

    # Add the gap after the pqrst when the heart is resting.
    samples_rest = 10
    zero_array = numpy.zeros(samples_rest, dtype=float)
    pqrst_full = numpy.concatenate([pqrst, zero_array])
    pqrst_full = pqrst_full[:12]

    # Simulated Beats per minute rate
    # For a health, athletic, person, 60 is resting, 180 is intensive exercising
    bpm = 60
    bps = bpm / 60

    # Simumated period of time in seconds that the ecg is captured in
    capture_length = 1

    # Caculate the number of beats in capture time period
    # Round the number to simplify things
    num_heart_beats = int(capture_length * bps)

    # Concatonate together the number of heart beats needed
    ecg_template = numpy.tile(pqrst_full, num_heart_beats)

    # Add random (gaussian distributed) noise
    noise = numpy.random.normal(0, 0.01, len(ecg_template))
    ecg_template_noisy = noise + ecg_template

    # Simulate an ADC by sampling the noisy ecg template to produce the values
    # Might be worth checking nyquist here
    # e.g. sampling rate >= (2 * template sampling rate)
    sampling_rate = 50.0
    num_samples = sampling_rate * capture_length
    ecg_sampled = signal.resample(ecg_template_noisy, int(num_samples))

    # Scale the normalised amplitude of the sampled ecg to whatever the ADC
    # bit resolution is
    # note: check if this is correct: not sure if there should be negative bit values.
    adc_bit_resolution = 1024
    ecg = adc_bit_resolution * ecg_sampled
    return ecg


def generate_type_3_wave():
    arr = [r(-10, 10), r(90, 110), r(-60, -40), r(190, 210), r(290, 310), r(-410, -390), r(290, 310), r(90, 110), r(0, 20)]
    arr_sampled = signal.resample(arr, 70)

    return arr_sampled

def liveGenerating(subor):
    size = 100
    x_vec = numpy.linspace(0, 1, size + 1)[0:-1]
    y_vec = numpy.random.randn(len(x_vec))
    line1 = []
    array = []
    file = open("prikazy/prikazy_"+subor, "r")

    while 1:
        cPrikaz = file.readline()
        if cPrikaz.__contains__("w1"):
            wave = generate_type_1_wave()
            for w in wave:
                array.append(w)

        if cPrikaz.__contains__("w2"):
            cPrikaz = ""
            #arr = [1000, 2500, 1700, 300, 1000]
            arr = [random.randint(950, 1050), random.randint(1950, 2050),
                   random.randint(1650, 1750),random.randint(250, 350), random.randint(950, 1050)]
            arr_sampled = signal.resample(arr, 30)
            arr_sampled *= -1
            arr_sampled += 1000
            for k in arr_sampled:
                array.append(k)

        if cPrikaz.__contains__("w3"):
            arr = [r(-10, 10), r(90,110), r(-60, -40), r(190, 210), r(290, 310), r(-410, -390), r(290,310), r(90,110), r(0, 20)]
            arr_sampled = signal.resample(arr, 70)
            for w in arr_sampled:
                array.append(w)

        if cPrikaz.__contains__("a"):
            cPrikaz = ""
            aType = random.randint(0,4)
            if aType == 0:
                arr = numpy.random.normal(0, 100, random.randint(15, 50))
            elif aType == 1:
                arr = numpy.random.normal(0, 500, random.randint(15, 50))
            elif aType == 2:
                arr = numpy.random.normal(0, 1000, random.randint(15, 50))
            elif aType == 3:
                arr = numpy.random.normal(0, 1300, random.randint(15, 50))
            else:
                arr = numpy.random.normal(0, 700, random.randint(15, 50))
            for k in arr:
                array.append(k)

        if cPrikaz.__contains__("q"):
            foutMeranie = open("/Users/dejvid/Codes/DP/DP_peakAnalyzer/data/bluetoothData/" + subor, "a")
            foutMeranie.write(str(1100000) + "\n")
            foutMeranie.close()
            cPrikaz = ""
            break

        if len(array) > 0:
            y_vec[-1] = array[0]
            foutMeranie = open("/Users/dejvid/Codes/DP/DP_peakAnalyzer/data/bluetoothData/" + subor, "a")
            foutMeranie.write(str(array[0])+"\n")
            foutMeranie.close()
            array.pop(0)
        else:
            rand_val = numpy.random.normal(0, 20)
            y_vec[-1] = rand_val
            foutMeranie = open("/Users/dejvid/Codes/DP/DP_peakAnalyzer/data/bluetoothData/" + subor, "a")
            foutMeranie.write(str(rand_val) + "\n")
            foutMeranie.close()

        line1 = live_plotter(x_vec, y_vec, line1)
        y_vec = numpy.append(y_vec[1:], 0.0)


def generateXOR():
    fout = open("/Users/dejvid/Codes/DP/DP_peakAnalyzer/data/test_xor.csv", "w")
    for i in range(0, 100):
        a = random.randint(0,1)
        b = random.randint(0,1)
        if(a == b):
            c = 0
        else:
            c = 1;
        print(str(a)+","+str(b)+" : "+str(c))
        fout.write(str(a)+";"+str(b)+";"+str(c)+"\n")
    fout.close()







def generate_type_3():
    peaks = 10000
    fout = open("/Users/dejvid/Codes/DP/DP_peakAnalyzer/data/typ3_10000.csv", "a")

    for i in range(0, peaks):
        wave = generate_type_3_wave()
        for x in range(0, len(wave)):
            a = format(wave[x], ".4f")
            fout.write(str(a) + ";")
        fout.write("1\n")

    fout.close()


def generate_type_2():
    noiseLength = 30  # Kazdych X riadkov sa vytvori meranie
    peaks = 2
    meranie = []
    print("Starting generator\n")

    for i in range(0, peaks):

        noise = numpy.random.normal(0, 20, random.randint(15,50))
        for n in noise:
            meranie.append(n)

        artefact = numpy.random.normal(0, 1000, random.randint(15,50))
        for a in artefact:
            meranie.append(a)

        noise = numpy.random.normal(0, 20, random.randint(15,50))
        for n in noise:
            meranie.append(n)

        #arr = [random.randint(950, 1050), random.randint(1950, 2050), random.randint(1650, 1750), random.randint(250, 350), random.randint(950, 1050)]
        arr = [1000, 2500, 1700, 300, 1000]
        arr_sampled = signal.resample(arr, 30)
        arr_sampled *= -1
        arr_sampled += 1000
        for a in arr_sampled:
            meranie.append(a)


        noise = numpy.random.normal(0, 20, random.randint(15,50))
        for n in noise:
            meranie.append(n)

        artefact = numpy.random.normal(0, 300, random.randint(15,50))
        for a in artefact:
            meranie.append(a)



    noise = numpy.random.normal(0, 20, noiseLength)
    for n in noise:
        meranie.append(n)

    pylab.plot(meranie)
    pylab.show()

    fout = open("/Users/dejvid/Codes/DP/DP_peakAnalyzer/data/typ2_artf.txt", "w")
    for i in range(0, len(meranie)):
        fout.write(str(meranie[i])+"\n")
    fout.close()

    print("Measurement generated")



def generate_type_1():
    # CONFIG
    MEASURE_COUNT = 1  # pocet merani
    SAMPLIG_RATE = 150.0  # ako nahusto budu od seba jednotlive hodnoty


    fout = open("/Users/dejvid/Codes/DP/DP_peakAnalyzer/data/typ1.txt", "w")
    #fout.write(str(MEASURE_COUNT) + "\n")

    meranie = []
    # vygenerovanie signalu
    samples_rest = 10
    zero_array = numpy.zeros(samples_rest, dtype=float)

    # The "Daubechies" wavelet is a rough approximation to a real,
    # single, heart beat ("pqrst") signal
    pqrst = signal.wavelets.daub(10)

    # Add the gap after the pqrst when the heart is resting.
    pqrst_full = numpy.concatenate([zero_array,pqrst])

    # Simulated Beats per minute rate
    # For a health, athletic, person, 60 is resting, 180 is intensive exercising
    bpm = 60
    bps = bpm / 60

    # Simumated period of time in seconds that the ecg is captured in
    capture_length = MEASURE_COUNT

    # Caculate the number of beats in capture time period
    # Round the number to simplify things
    num_heart_beats = int(capture_length * bps)

    # Concatonate together the number of heart beats needed
    ecg_template = numpy.tile(pqrst_full, num_heart_beats)

    # Add random (gaussian distributed) noise
    noise = numpy.random.normal(0, 0.01, len(ecg_template))
    ecg_template_noisy = noise + ecg_template

    # Simulate an ADC by sampling the noisy ecg template to produce the values
    # Might be worth checking nyquist here
    # e.g. sampling rate >= (2 * template sampling rate)
    sampling_rate = SAMPLIG_RATE
    num_samples = sampling_rate * capture_length
    ecg_sampled = signal.resample(ecg_template_noisy, int(num_samples))

    # Scale the normalised amplitude of the sampled ecg to whatever the ADC
    # bit resolution is
    # note: check if this is correct: not sure if there should be negative bit values.
    adc_bit_resolution = 1024
    ecg = adc_bit_resolution * ecg_sampled

    for x in ecg:
        fout.write(str(x) + "\n")
        meranie.append(x)
    fout.close()

    print("Measurement generated")
    print("Number of measures: " + str(MEASURE_COUNT) + "\n")
    pylab.plot(meranie)
    pylab.show()




def generate_noise_toFile():

    for i in range(0,100):
        fout = open("noise"+str(i)+".txt", "w")
        noise = numpy.random.normal(0, 20, 51)
        for x in noise:
            fout.write(str(x) + "\n")
        fout.close()
        print("Vytvoreny subor noise"+str(i)+".txt")


def append_noise(length):
    lines = []
    with open("/Users/dejvid/Codes/DP/DP_peakAnalyzer/data/typ3_10000.csv", 'r') as file:
        lines = file.readlines()
    file.close()

    noised_range = [0, len(lines) - 1] # rozsah riadkov kde sa bude generovat sum

    for i in range(0, len(lines)):
        i = 0
        noise = numpy.random.normal(0, 20, length)
        lineNum = random.randint(noised_range[0], noised_range[1])
        print(lineNum)

        line = ""
        for c in noise:
            c = "%.4f" % c
            l = str(c).replace('.', ',')
            line += l + ";"

        line += "0\n"
        print(line)

        lines.insert(lineNum, line)
        noised_range[1] += 1

    with open("/Users/dejvid/Codes/DP/DP_peakAnalyzer/data/noised_typ3_10000.csv", 'w') as file:
        for l in lines:
            file.write(l)
    file.close()



def createTrainAndTestFiles(sourceFile, trainFile, testFile, validateFile):
    data_path = "/Users/dejvid/Codes/DP/DP_peakAnalyzer/data/"
    lines = []
    trainSet = []
    testSet = []
    valSet = []

    sourceFile = data_path + sourceFile
    trainFile = data_path + trainFile
    testFile = data_path + testFile
    validateFile = data_path + validateFile

    with open(sourceFile, "r") as inFile:
        lines = inFile.readlines()
    inFile.close()

    shuffle(lines)

    linesLen = len(lines)
    trainSetLen = (linesLen / 3) * 2
    testSetLen = ((linesLen - int(trainSetLen)) / 4 ) * 3

    for x in range(0, int(trainSetLen)):
        trainSet.append(lines[x])

    with open(trainFile, "w") as outFile:
        for l in trainSet:
            outFile.write(l)
    outFile.close()

    for x in range(int(trainSetLen), int(trainSetLen + testSetLen)):
        testSet.append(lines[x])

    with open(testFile, "w") as outFile:
        for l in testSet:
            outFile.write(l)
    outFile.close()

    for x in range(int(trainSetLen + testSetLen), int(linesLen)):
        valSet.append(lines[x])

    with open(validateFile, "w") as outFile:
        for l in valSet:
            outFile.write(l)
    outFile.close()


#generate_type_3()
#append_noise(70)
#createTrainAndTestFiles("noised_typ3_10000.csv", "train_wave3.csv", "test_wave3.csv", "val_wave3.csv")

#generate_type_2()
#generate_type_1()




liveGenerating(sys.argv[1])

