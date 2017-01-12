from __future__                 import division
from   sklearn.decomposition    import PCA                as pca
from   sklearn.preprocessing    import StandardScaler     as stdScaler
from   sklearn.cross_validation import train_test_split   as cvSplit
from   sklearn.linear_model     import LogisticRegression as lReg
from   sklearn.cross_validation import KFold              as kFold
from   sklearn.cross_validation import cross_val_score    as cValScore
from   sklearn.metrics          import confusion_matrix   as confMat
from   sklearn.metrics          import precision_recall_curve as precRecallCurve
from   sklearn.metrics          import auc                    as auc
from   sklearn.metrics          import roc_auc_score          as rocAucScore
from   sklearn.metrics          import roc_curve              as rocCurve
from   sklearn.metrics          import recall_score           as recallScore
from   sklearn.metrics          import classification_report  as classificationReport

import pandas as pd, matplotlib.pyplot as plt, numpy as np, itertools as it

def doUnderSample(data, verbose=False):
    noOfFrauds         = len(data[data.Class == 1])
    fraudIndices       = np.array(data[data.Class == 1].index)
    normalIndices      = data[data.Class == 0].index
    rNormalIndices     = np.random.choice(normalIndices, noOfFrauds, replace = False)
    rNormalIndices     = np.array(rNormalIndices)
    underSampleIndices = np.concatenate([fraudIndices, rNormalIndices])
    underSampleData    = data.iloc[underSampleIndices,:]
    xUnderSample       = underSampleData.ix[:, underSampleData.columns != 'Class']
    yUnderSample       = underSampleData.ix[:, underSampleData.columns == 'Class']
    if verbose:
        print("Normal Transactions in Undersampled data: ", len(underSampleData[underSampleData.Class == 0]))
        print("Fraud Transactions in Undersampled data", len(underSampleData[underSampleData.Class == 1]))
    return xUnderSample, yUnderSample

def doKFoldScores(xTrainData, yTrainData, pandasHelper=pd, numpyHelper=np, verbose=False):
    fold         = kFold(len(yTrainData), 5, shuffle=False)
    cParamRange  = [0.01, 0.1, 1, 10, 100]
    resultsTable = pandasHelper.DataFrame(index=range(len(cParamRange), 2), columns=['C_parameter','Mean recall score'])
    resultsTable['C_parameter'] = cParamRange

    j = 0
    for cParam in cParamRange:
        recallAccums = []
        for iteration, indices in enumerate(fold, start=1):
            lr = lReg(C=cParam, penalty='l1')
            lr.fit(xTrainData.iloc[indices[0],:], yTrainData.iloc[indices[0],:].values.ravel())
            yPredUnderSample = lr.predict(xTrainData.iloc[indices[1],:].values)
            llAccum = recallScore(yTrainData.iloc[indices[1],:].values, yPredUnderSample)
            recallAccums.append(llAccum)
            if verbose: print('# Iteration ', iteration,': recall score = ', recallAccum)

        resultsTable.ix[j, 'Mean recall score'] = numpyHelper.mean(recallAccums)
        if verbose: print('# --> Mean recall score ', numpyHelper.mean(recallAccums))
        j += 1

    bestC = resultsTable.loc[resultsTable['Mean recall score'].idxmax()]['C_parameter']
    if verbose:
        print('*********************************************************************************')
        print('Best model to choose from cross validation is with C parameter = ', bestC)
        print('*********************************************************************************')
    return bestC

def printCVDataStats(xTrain, xTest, xTrainUnderSample, xTestUnderSample):
    print("Transactions in Training: ", len(xTrain))
    print("Transactions in Testing: ", len(xTest))
    print("Transactions in UnderSample Training: ", len(xTrainUnderSample))
    print("Transactions in UnderSample Testing: ", len(xTestUnderSample))


def drawImblalancedTrainingSet(countClasses, plotHelper=plt):
    countClasses.plot(kind='bar')
    plotHelper.title("Fraud class histogram")
    plotHelper.xlabel("Class")
    plotHelper.ylabel("Frequency")

def drawConfusionMatrix(yData, yPred, plotHelper=plt, numpyHelper=np, verbose=False):
    cnfMatrix = confMat(yData, yPred)
    numpyHelper.set_printoptions(precision=2)
    if verbose: print("Recall metric in the testing dataset: ", cnfMatrix[1,1]/(cnfMatrix[1,0]+cnfMatrix[1,1]))
    classNames = [0,1]
    plotHelper.figure()
    plotConfusionMatrix(cnfMatrix, classes=classNames, title='Confusion Matrix::Recall: %0.5f' \
                     % ((cnfMatrix[1,1]*100.0)/(cnfMatrix[1,0]+cnfMatrix[1,1])))

def plotConfusionMatrix(cm, classes, normalize=False, title='Confusion Matrix', plotHelper=plt, numpyHelper=np):
    plotHelper.imshow(cm, interpolation='nearest', cmap=plotHelper.cm.Blues)
    plotHelper.title(title)
    plotHelper.colorbar()
    tickMarks = numpyHelper.arange(len(classes))
    plotHelper.xticks(tickMarks, classes, rotation=0)
    plotHelper.yticks(tickMarks, classes)
    if normalize: cm = cm.astype('float') / cm.sum(axis=1)[:, numpyHelper.newaxis]
    else: 1
    thresh = cm.max() / 2.
    for i, j in it.product(range(cm.shape[0]), range(cm.shape[1])):
        plotHelper.text(j, i, cm[i, j], horizontalalignment="center", color="white" if cm[i, j]>thresh else "black")
    plotHelper.tight_layout()
    plotHelper.ylabel('True label')
    plotHelper.xlabel('Predicted label')


# --------------------------------------------------

# Read, anonymise and undersample data
data                       = pd.read_csv("./creditcard.csv")
countClasses               = pd.value_counts(data['Class'], sort = True).sort_index()
pcaHelper                  = pca(n_components=1)
data['PCA']                = pcaHelper.fit_transform(data.as_matrix(columns=['Time', 'Amount']))
data                       = data.drop(['Time','Amount'], axis=1)
X                          = data.ix[:, data.columns != 'Class']
Y                          = data.ix[:, data.columns == 'Class']
xUnderSample, yUnderSample = doUnderSample(data)

# Create and print stats for cross validation data pieces
xTrain, xTest, yTrain, yTest = cvSplit(X, Y, test_size = 0.3, random_state = 0)
xTrainUnderSample, xTestUnderSample, yTrainUnderSample, yTestUnderSample = cvSplit(xUnderSample,    \
                                                                                   yUnderSample,    \
                                                                                   test_size=0.3,   \
                                                                                   random_state=0   )
# printCVDataStats(xTrain, xTest, xTrainUnderSample, xTestUnderSample)

bestC                 = doKFoldScores(xTrainUnderSample, yTrainUnderSample)
lr                    = lReg(C=bestC, penalty='l2')
lr.fit(xTrainUnderSample, yTrainUnderSample.values.ravel())
yPredUnderSample      = lr.predict(xTestUnderSample.values)
yPred                 = lr.predict(xTest.values)
lr.fit(xTrain, yTrain.values.ravel())
yPredNoUnderSample    = lr.predict(xTest.values)

# Show the degree of imbalance in the dataset
drawImblalancedTrainingSet(countClasses)

# Show > 90% recall in training
drawConfusionMatrix(yTestUnderSample, yPredUnderSample)

# Show > 90% recall in training
drawConfusionMatrix(yTest, yPred)

# Show horrible % recall if undersampling is not done
drawConfusionMatrix(yTest, yPredNoUnderSample)

# Retrain with undersample and show precision vs recall trade off (Increasing normal transaction marked as fraud)
lr.fit(xTrainUnderSample, yTrainUnderSample.values.ravel())
yPredUnderSampleProb = lr.predict_proba(xTestUnderSample.values)
thresholds           = np.arange(0.1, 1, 0.1)
plt.figure(figsize=(10,10))
for j, i in enumerate(thresholds, start=1):
    yTestPredHighRecall = yPredUnderSampleProb[:,1] > i
    drawConfusionMatrix(yTestUnderSample, yTestPredHighRecall)

# Show any plots if drawn
# plt.show()
