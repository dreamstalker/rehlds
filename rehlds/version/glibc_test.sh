#!/bin/bash

main()
{
	files=($@)

	declare -A threshold_version
	threshold_version[CXXABI]="1.3.5"
	threshold_version[GLIBCXX]="3.4.15"
	threshold_version[GLIBC]="2.11"

	for k in "${!threshold_version[@]}"; do
		for f in "${files[@]}"
		do
			:
			version=$(readelf -sV $f | sed -n 's/.*@'$k'_//p' | sort -u -V | tail -1 | cut -d ' ' -f 1)

			# version no present - skipped
			if [[ -z "$version" ]]; then
				version="UND"
			# version is private - skipped
			elif [ "$version" = "PRIVATE" ]; then
				version="PRV"
			# ensure numeric
			elif [[ $version =~ ^([0-9]+\.){0,2}(\*|[0-9]+)$ ]]; then
				check_version_greater $version ${threshold_version[$k]}
				if [[ $? -eq 1 ]]; then
					echo -e "\033[0;31mAssertion failed:\033[0m Binary \033[0;32m${f}\033[0m has ${k}_\033[0;33m$version\033[0m greater than max version ${k}_\033[0;33m${threshold_version[$k]}\033[0m"
					exit -1
				fi
			fi
		done

		if [[ "$version" = "PRV" || "$version" = "UND" ]]; then
			echo -e "[\033[0;90mSKIP\033[0m] \033[0;33m${version}\033[0m < ${k}_\033[0;33m${threshold_version[$k]}\033[0m"
		else
			echo -e "[\033[0;32mOK\033[0m] \033[0;33m${version}\033[0m < ${k}_\033[0;33m${threshold_version[$k]}\033[0m"
		fi
	done
}

check_version_greater()
{
	if [[ -z "$1" || $1 == $2 ]]; then
		return 0
	fi

	local IFS=.
	local i ver1=($1) ver2=($2)

	# fill empty fields in ver1 with zeros
	for ((i = ${#ver1[@]}; i < ${#ver2[@]}; i++))
	do
		ver1[i]=0
	done

	for ((i = 0; i < ${#ver1[@]}; i++))
	do
		if [[ -z ${ver2[i]} ]]
		then
			# fill empty fields in ver2 with zeros
			ver2[i]=0
		fi

		if ((10#${ver1[i]} > 10#${ver2[i]}))
		then
			return 1
		fi

		if ((10#${ver1[i]} < 10#${ver2[i]}))
		then
			break
		fi
	done

	return 0
}

# Initialize
main $*

# Exit normally
exit 0
