#!/bin/bash

init()
{
	SOURCE_DIR=$@
	GIT_DIR=$SOURCE_DIR/..
	VERSION_FILE=$GIT_DIR/gradle.properties
	APPVERSION_FILE=$SOURCE_DIR/version/appversion.h

	if test -z "`git --version`"; then
		echo "Please install git client"
		echo "sudo apt-get install git"
		exit -1
	fi

	# Read old version
	if [ -e $APPVERSION_FILE ]; then
		OLD_VERSION=$(cat $APPVERSION_FILE | grep -wi '#define APP_VERSION' | sed -e 's/#define APP_VERSION[ \t\r\n\v\f]\+\(.*\)/\1/i' -e 's/\r//g')
		if [ $? -ne 0 ]; then
			OLD_VERSION=""
		else
			# Remove quotes
			OLD_VERSION=$(echo $OLD_VERSION | xargs)
		fi
	fi

	# Get major, minor and maintenance information from gradle.properties
	MAJOR=$(sed -nr -e '/majorVersion/ s/.*\= *//p' "$VERSION_FILE" | tr -d '\n\r')
	if [ $? -ne 0 -o "$MAJOR" = "" ]; then
		MAJOR=0
	fi

	MINOR=$(sed -nr -e '/minorVersion/ s/.*\= *//p' "$VERSION_FILE" | tr -d '\n\r')
	if [ $? -ne 0 -o "$MINOR" = "" ]; then
		MINOR=0
	fi

	MAINTENANCE=$(sed -nr -e '/maintenanceVersion/ s/.*\= *//p' "$VERSION_FILE" | tr -d '\n\r')
	if [ $? -ne 0 -o "$MAINTENANCE" = "" ]; then
		MAINTENANCE=0
	fi

	BRANCH_NAME=$(git -C "$GIT_DIR/" rev-parse --abbrev-ref HEAD)
	if [ $? -ne 0 -o "$BRANCH_NAME" = "" ]; then
		BRANCH_NAME=master
	fi

	COMMIT_COUNT=$(git -C "$GIT_DIR/" rev-list --count $BRANCH_NAME)
	if [ $? -ne 0 -o "$COMMIT_COUNT" = "" ]; then
		COMMIT_COUNT=0
	fi

	#
	# Configure remote url repository
	#
	# Get remote name by current branch
	BRANCH_REMOTE=$(git -C "$GIT_DIR/" config branch.$BRANCH_NAME.remote)
	if [ $? -ne 0 -o "$BRANCH_REMOTE" = "" ]; then
		BRANCH_REMOTE=origin
	fi

	# Get commit id
	COMMIT_SHA=$(git -C "$GIT_DIR/" rev-parse --verify HEAD)
	COMMIT_SHA=${COMMIT_SHA:0:7}

	# Get remote url
	COMMIT_URL=$(git -C "$GIT_DIR/" config remote.$BRANCH_REMOTE.url)

	URL_CONSTRUCT=0

	if [[ "$COMMIT_URL" == *"git@"* ]]; then
		URL_CONSTRUCT=1

		# Strip prefix 'git@'
		COMMIT_URL=${COMMIT_URL#git@}

		# Strip postfix '.git'
		COMMIT_URL=${COMMIT_URL%.git}

		# Replace ':' to '/'
		COMMIT_URL=${COMMIT_URL/:/\/}

	elif [[ "$COMMIT_URL" == *"https://"* ]]; then
		URL_CONSTRUCT=1

		# Strip prefix 'https://'
		COMMIT_URL=${COMMIT_URL#https://}

		# Strip postfix '.git'
		COMMIT_URL=${COMMIT_URL%.git}
	fi

	if test "$URL_CONSTRUCT" -eq 1; then
		# Append extra string
		if [[ "$COMMIT_URL" == *"bitbucket.org"* ]]; then
			COMMIT_URL=$(echo https://$COMMIT_URL/commits/)
		else
			COMMIT_URL=$(echo https://$COMMIT_URL/commit/)
		fi
	fi

	#
	# Detect local modifications
	#
	if [ `git -C "$GIT_DIR/" ls-files -m | wc -l` = 0 ]; then
		MODIFIED=
	else
		MODIFIED=+m
	fi

	NEW_VERSION="$MAJOR.$MINOR.$MAINTENANCE.$COMMIT_COUNT-dev$MODIFIED"

	# Update appversion.h if version has changed or modifications/mixed revisions detected
	if [ "$NEW_VERSION" != "$OLD_VERSION" ]; then
		update_appversion
	fi
}

update_appversion()
{
	day=$(date +%m)
	year=$(date +%Y)
	hours=$(date +%H:%M:%S)
	month=$(LANG=en_us_88591; date +"%b")

	# Write appversion.h
	echo Updating appversion.h, new version is '"'$NEW_VERSION'"', the old one was $OLD_VERSION

	echo -e "#ifndef __APPVERSION_H__\r">"$APPVERSION_FILE"
	echo -e "#define __APPVERSION_H__\r">>"$APPVERSION_FILE"
	echo -e "\r">>"$APPVERSION_FILE"
	echo -e "//\r">>"$APPVERSION_FILE"
	echo -e "// This file is generated automatically.\r">>"$APPVERSION_FILE"
	echo -e "// Don't edit it.\r">>"$APPVERSION_FILE"
	echo -e "//\r">>"$APPVERSION_FILE"
	echo -e "\r">>"$APPVERSION_FILE"
	echo -e "// Version defines\r">>"$APPVERSION_FILE"
	echo -e '#define APP_VERSION "'$NEW_VERSION'"\r'>>"$APPVERSION_FILE"

	echo -e "#define APP_VERSION_C $MAJOR,$MINOR,$MAINTENANCE,$COMMIT_COUNT\r">>"$APPVERSION_FILE"
	echo -e '#define APP_VERSION_STRD "'$MAJOR.$MINOR.$MAINTENANCE.$COMMIT_COUNT'"\r'>>"$APPVERSION_FILE"
	echo -e "#define APP_VERSION_FLAGS 0x0L\r">>"$APPVERSION_FILE"
	echo -e "\r">>"$APPVERSION_FILE"
	echo -e '#define APP_COMMIT_DATE "'$month $day $year'"\r'>>"$APPVERSION_FILE"
	echo -e '#define APP_COMMIT_TIME "'$hours'"\r'>>"$APPVERSION_FILE"
	echo -e "\r">>"$APPVERSION_FILE"

	echo -e '#define APP_COMMIT_SHA "'$COMMIT_SHA'"\r'>>"$APPVERSION_FILE"
	echo -e '#define APP_COMMIT_URL "'$COMMIT_URL'"\r'>>"$APPVERSION_FILE"
	echo -e "\r">>"$APPVERSION_FILE"
	echo -e "#endif //__APPVERSION_H__\r">>"$APPVERSION_FILE"
}

# Initialise
init $*

# Exit normally
exit 0
