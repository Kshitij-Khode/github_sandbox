var app = angular.module('ITR', ['ui.bootstrap','angular-loading-bar','ngAnimate']);

app.controller('TopLevelCtrl', function($scope, $modal, $http){

	var init = function() {
		// Function to initialize view
		$scope.runIDSearch = '';
		$scope.dateSearch = '';
		$scope.buildSearch = '';
		$scope.platformSearch = '-ALL-';
		$scope.testHostSearch = '-ALL-';
		$scope.tunnelTypeSearch = '-ALL-';
		$scope.tagSearch = '-ALL-';

		// Request data
		var url = '/findInSummaries/30';
		$http.get(url).success(function(data, status, headers, config){
			if (status == 200) {
				// Get new data
				data.sort(dateSortAsc);
				$scope.summaryData = data;
			}
		}).error(function(data, status, headers, config){
			alert('Error(' + status + '): ' + data);
		});
	}

	var dateSortAsc = function(obj1, obj2) {
		if (obj1.date > obj2.date) return 1;
		if (obj1.date < obj2.date) return -1;
		return 0;
	}

	$scope.$watch('summaryDataFiltered.length', function(newValue, oldValue){
		// request new data if the number of rows are below threshold value
		if (newValue < 25) {
			// Request more data
			var url = '/findInSummaries/10?date[$lt]=' + $scope.summaryData[0].date;
			$http.get(url).success(function(data, status, headers, config){
				if (status == 200) {
					// Append to existing data
					for (var i in data) {
						var pass = true;
						for (var j in $scope.summaryData) {
							if ($scope.summaryData[j].run_id == data[i].run_id) {
								pass = false;
								break;
							}
						}
						if (pass) {
							$scope.summaryData.push(data[i]);
						}
					}
					$scope.summaryData.sort(dateSortAsc);
				}
			}).error(function(data, status, headers, config){
				alert('Error(' + status + '): ' + data);
			});
		}
	});

	$scope.openDetailView = function(testRe, rawTestNu) {
		// testRe - Run ID
		// rawTestNu - Section Text
		testNu = rawTestNu.split(",")[0];
		var modalInstance = $modal.open({
			templateUrl: 'DetailLevel.html',
			controller: 'DetailLevelCtrl',
			windowClass: 'DetailModalWindow',
			resolve: {
				items: function() {
					return {
						run_id : testRe,
						section_id : testNu
					}
				}
			}
		});

		modalInstance.result.then(function() {
			// Dismissed using OK button
		}, function() {
			// Dismissed using Cancel button
		});
	}

	$scope.getTestTypes = function() {
		var data = [];
		for (var ind in $scope.summaryData) {
			for (var indI in $scope.summaryData[ind].sections) {
				var temp = "" + $scope.summaryData[ind].sections[indI].section_id + "," +
							$scope.summaryData[ind].sections[indI].section_name + "";
				if (data.indexOf(temp) < 0) {
					data.push(temp);
				}
			}
		}
		return data;
	}

	$scope.sortTestTypes = function(section) {
		return parseInt(section.split(",")[0]);
	}

	$scope.getTags = function() {
		var data = [];
		for (var tagArr in $scope.summaryData) {
			for (var tag in $scope.summaryData[tagArr].tags) {
				if (data.indexOf($scope.summaryData[tagArr].tags[tag]) < 0) {
					data.push($scope.summaryData[tagArr].tags[tag]);
				}
			}
		}
		return data.sort();
	}

	$scope.getPlatforms = function() {
		var data = [];
		for (var pla in $scope.summaryData) {
			if (data.indexOf($scope.summaryData[pla].platform) < 0) {
				data.push($scope.summaryData[pla].platform);
			}
		}
		return data.sort();
	}

	$scope.getTestHosts = function() {
		var data = [];
		for (var test in $scope.summaryData) {
			if (data.indexOf($scope.summaryData[test].test_host) < 0) {
				data.push($scope.summaryData[test].test_host);
			}
		}
		return data.sort();
	}

	$scope.getTunnelTypes = function() {
		var data = [];
		for (var tunn in $scope.summaryData) {
			if (data.indexOf($scope.summaryData[tunn].tunnel_type) < 0) {
				data.push($scope.summaryData[tunn].tunnel_type);
			}
		}
		return data.sort();
	}

	$scope.dropdownFilterStrict = function(actual, expected) {
		if (expected == '-ALL-') {
			return true;
		} else {
			return angular.equals(expected, actual);
		}
	}

	$scope.dropdownFilter = function(actual, expected) {
		if (expected == '-ALL-') {
			return true;
		} else if (String(actual).indexOf(String(expected)) >= 0) {
			return true;
		}
	}

	$scope.setTestDetailBtnClass = function(sections, section) {
		var status;
		// Find percentage
		for (var i in sections) {
			if (sections[i].section_id == section.split(",")[0]) {
				status = sections[i].pass_percentage;
			}
		}
		// Set button class
		if (status < 100 && status > 75) {
			return 'btn btn-primary'
		} else if (status <= 75 && status > 50) {
			return 'btn btn-info'
		} else if (status <= 50 && status > 25) {
			return 'btn btn-warning'
		} else if (status <= 25 && status >= 0) {
			return 'btn btn-danger'
		} else if (status == 100) {
			return 'btn btn-success'
		}
	}

	$scope.getPassPerString = function(sections, section) {
		var rawPer;
		// Find percentage
		for (var i in sections) {
			if (sections[i].section_id == section.split(",")[0]) {
				rawPer = sections[i].pass_percentage;
			}
		}
		return Math.round(rawPer);
	}

	$scope.checkMissingTest = function(sections, section) {
		var found = false;
		for (var i in sections) {
			if (sections[i].section_id == section.split(",")[0]) {
				found = true;
			}
		}
		return found;
	}

	// Calling function to initialize view
	init();
});