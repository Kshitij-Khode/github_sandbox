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
from   imblearn.over_sampling   import SMOTE                  as smote
from   sklearn.naive_bayes      import GaussianNB             as gaussianNB
from   sklearn.metrics          import roc_auc_score          as rocAucScore
from   sklearn.ensemble         import RandomForestClassifier as rfc

import pandas as pd, matplotlib.pyplot as plt, numpy as np, itertools as it, random

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
        print("Normal transactions in undersampled data: ", len(underSampleData[underSampleData.Class == 0]))
        print("Fraud transactions in undersampled data", len(underSampleData[underSampleData.Class == 1]))
    return xUnderSample, yUnderSample

def doOverSample(X, Y, verbose=False):
    xOverSample, yOverSample   = smote(random_state=0).fit_sample(X, Y)
    if verbose:
        print("Normal transactions in oversampled data: ", len([normal for normal in yOverSample if normal == 0]))
        print("Fraud transactions in oversampled data: ", len([fraud for fraud in yOverSample if fraud == 1]))
    xOverSample                = [xOverSample[:,column] for column in range(len(xOverSample[0]))]
    xOverSample                = { key: value for key, value in zip([xLabel for xLabel in X.keys()], xOverSample) }
    yOverSample                = { key: value for key, value in zip([yLabel for yLabel in Y.keys()], [yOverSample]) }
    return pd.DataFrame(xOverSample), pd.DataFrame(yOverSample)

def doSampling(data, X, Y, type=0):
    if type == 0: return doUnderSample(data)
    else:         return doOverSample(X, Y)

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
            yPredSampled = lr.predict(xTrainData.iloc[indices[1],:].values)
            llAccum = recallScore(yTrainData.iloc[indices[1],:].values, yPredSampled)
            recallAccums.append(llAccum)
            if verbose: print('# Iteration ', iteration,': recall score = ', llAccum)

        resultsTable.ix[j, 'Mean recall score'] = numpyHelper.mean(recallAccums)
        if verbose: print('# --> Mean recall score ', numpyHelper.mean(recallAccums))
        j += 1

    bestC = resultsTable.loc[resultsTable['Mean recall score'].idxmax()]['C_parameter']
    if verbose:
        print('*********************************************************************************')
        print('Best model to choose from cross validation is with C parameter = ', bestC)
        print('*********************************************************************************')
    return bestC

def printCVDataStats(xTrain, xTest, xTrainSampled, xTestSampled):
    print("Transactions in Training: ", len(xTrain))
    print("Transactions in Testing: ", len(xTest))
    print("Transactions in Sampled Training: ", len(xTrainSampled))
    print("Transactions in Sampled Testing: ", len(xTestSampled))


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

def drawRocCurve(yTestSampled, yPredSampledScore, plotHelper=plt):
    fpr, tpr, thresholds = rocCurve(yTestSampled.values.ravel(), yPredSampledScore)
    rocAuc               = auc(fpr,tpr)
    plotHelper.title('Receiver Operating Characteristic')
    plotHelper.plot(fpr, tpr, 'b',label='AUC = %0.2f'% rocAuc)
    plotHelper.legend(loc='lower right')
    plotHelper.plot([0,1],[0,1],'r--')
    plotHelper.xlim([-0.1,1.0])
    plotHelper.ylim([-0.1,1.01])
    plotHelper.ylabel('True Positive Rate')
    plotHelper.xlabel('False Positive Rate')

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

# Read, anonymise and sample data
data                       = pd.read_csv("./creditcard.csv")
countClasses               = pd.value_counts(data['Class'], sort = True).sort_index()
pcaHelper                  = pca(n_components=1)
data['PCA']                = pcaHelper.fit_transform(data.as_matrix(columns=['Time', 'Amount']))
data                       = data.drop(['Time','Amount'], axis=1)
X                          = data.ix[:, data.columns != 'Class']
Y                          = data.ix[:, data.columns == 'Class']


# Choose execution type (0, 1) with undersampled or oversampled data
xSampled, ySampled = doSampling(data, X, Y, type=0)

# Create and print stats for cross validation data pieces
xTrain, xTest, yTrain, yTest = cvSplit(X, Y, test_size=random.random(), random_state = 0)
xTrainSampled, xTestSampled, yTrainSampled, yTestSampled = cvSplit(xSampled,                    \
                                                                   ySampled,                    \
                                                                   test_size=random.random(),   \
                                                                   random_state=0)
# printCVDataStats(xTrain, xTest, xTrainSampled, xTestSampled)

# Do K-Fold to find optimal parameter for L2 regularization, train LR and test
bestC                 = doKFoldScores(xTrainSampled, yTrainSampled, verbose=True)
lr                    = lReg(C=bestC, penalty='l2')
lr.fit(xTrainSampled, yTrainSampled.values.ravel())
yPredSampled          = lr.predict(xTestSampled.values)
yPred                 = lr.predict(xTest.values)
lr.fit(xTrain, yTrain.values.ravel())
yPredNoSampled        = lr.predict(xTest.values)

# Show the degree of imbalance in the dataset
# drawImblalancedTrainingSet(countClasses)

# Show > 90% recall in training
drawConfusionMatrix(yTestSampled, yPredSampled)

# Show > 90% recall in testing
drawConfusionMatrix(yTest, yPred)

# Show horrible % recall if sampling is not done
# drawConfusionMatrix(yTest, yPredNoSampled)

# Prep for drawing ROC curve
yPredSampledScore = lr.fit(xTrainSampled, yTrainSampled.values.ravel()).decision_function(xTestSampled.values)

# Show TP vs FP tradeoff
# drawRocCurve(yTestSampled, yPredSampledScore)

# Show precision vs recall trade off with manually changing thresholds (Normal transaction marked as fraud increasingly)
# yPredSampledProb = lr.predict_proba(xTestSampled.values)
# thresholds       = np.arange(0.1, 1, 0.1)
# plt.figure(figsize=(10,10))
# for j, i in enumerate(thresholds, start=1):
#     yTestPredHighRecall = yPredSampledProb[:,1] > i
#     drawConfusionMatrix(yTestSampled, yTestPredHighRecall)

# Do naive bayes and compare
# nb             = gaussianNB()
# nb.fit(xTrainSampled, yTrainSampled)
# yPredNBSampled = nb.predict(xTestSampled)

# Show < 80% recall and auc for roc in training. Dump naive bayes
# drawConfusionMatrix(yTestSampled, yPredNBSampled)
# print("Naive Bayes Roc AUC Score: %0.2f " % (rocAucScore(yTestSampled, yPredNBSampled)))

# Do random forest classifier and compare
# rfc             = rfc().fit(xTrainSampled, yTrainSampled)
# yPredRFCSampled = rfc.predict(xTestSampled)

# Show ~ 90% recall and auc for roc in training. Logistic regression is stil better/
# drawConfusionMatrix(yTestSampled, yPredRFCSampled)
# print("Random Forest Classifier Roc AUC Score: %0.2f " % (rocAucScore(yTestSampled, yPredRFCSampled)))

# Show any plots if drawn
plt.show()
