import pandas                                      as pd
import matplotlib.pyplot                           as plt
import numpy                                       as np
from   sklearn.preprocessing import StandardScaler as stdScaler

data         = pd.read_csv("./creditcard.csv")
data.head()
countClasses = pd.value_counts(data['Class'], sort = True).sort_index()

countClasses.plot(kind = 'bar')
plt.title("Fraud class histogram")
plt.xlabel("Class")
plt.ylabel("Frequency")

# Show plot regarding imbalanced training set
# plt.show()


data['normAmount'] = stdScaler().fit_transform(data['Amount'].reshape(-1, 1))
data               = data.drop(['Time','Amount'], axis=1)
data.head()

X             = data.ix[:, data.columns != 'Class']
Y             = data.ix[:, data.columns == 'Class']
noOfFrauds    = len(data[data.Class == 1])
fraudIndices  = np.array(data[data.Class == 1].index)
normalIndices = data[data.Class == 0].index

# Out of the indices we picked, randomly select "x" number (noOfFrauds)
random_normal_indices = np.random.choice(normalIndices, noOfFrauds, replace = False)
random_normal_indices = np.array(random_normal_indices)

# Appending the 2 indices
under_sample_indices = np.concatenate([fraudIndices,random_normal_indices])

# Under sample dataset
under_sample_data = data.iloc[under_sample_indices,:]

X_undersample = under_sample_data.ix[:, under_sample_data.columns != 'Class']
y_undersample = under_sample_data.ix[:, under_sample_data.columns == 'Class']

# Showing ratio
print("Percentage of normal transactions: ", len(under_sample_data[under_sample_data.Class == 0])/len(under_sample_data))
print("Percentage of fraud transactions: ", len(under_sample_data[under_sample_data.Class == 1])/len(under_sample_data))
print("Total number of transactions in resampled data: ", len(under_sample_data))