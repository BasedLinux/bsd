#!/usr/bin/env bash

set -eo pipefail

# stock path to avoid unexpected command versions
PATH="$(/usr/bin/getconf PATH)"

((NEW_NIX_FIRST_BUILD_UID=351))
((TEMP_NIX_FIRST_BUILD_UID=31000))

bsd_user_n() {
	printf "_bsdbld%d" "$1"
}

id_unavailable(){
	dscl . list /Users UniqueID | grep -E '\b'"$1"'\b' >/dev/null
}

any_bsdbld(){
	dscl . list /Users UniqueID | grep -E '\b_bsdbld' >/dev/null
}

dsclattr() {
	dscl . -read "$1" | awk "/$2/ { print \$2 }"
}

re_create_bsdbld_user(){
	local name uid

	name="$1"
	uid="$2"
	gid="$3"

	sudo /usr/bin/dscl . -create "/Users/$name" "UniqueID" "$uid"
	sudo /usr/bin/dscl . -create "/Users/$name" "IsHidden" "1"
	sudo /usr/bin/dscl . -create "/Users/$name" "NFSHomeDirectory" "/var/empty"
	sudo /usr/bin/dscl . -create "/Users/$name" "RealName" "Bsd build user $name"
	sudo /usr/bin/dscl . -create "/Users/$name" "UserShell" "/sbin/nologin"
	sudo /usr/bin/dscl . -create "/Users/$name" "PrimaryGroupID" "$gid"
}

hit_id_cap(){
	echo "We've hit UID 400 without placing all of your users :("
	echo "You should use the commands in this script as a starting"
	echo "point to review your UID-space and manually move the"
	echo "remaining users (or delete them, if you don't need them)."
}

# evacuate the role-uid space to simplify final placement logic
temporarily_move_existing_bsdbld_uids(){
	local name uid next_id user_n

	((next_id=TEMP_NIX_FIRST_BUILD_UID))

	echo ""
	echo "Step 1: move existing _bsdbld users out of the destination UID range."

	while read -r name uid; do
		# iterate for a clean ID
		while id_unavailable "$next_id"; do
			((next_id++))
			# We really want to get these all placed, but I guess there's
			# some risk we iterate forever--so we'll give up after 9k uids.
			if ((next_id >= 40000)); then
				echo "We've hit UID 40000 without temporarily placing all of your users :("
				echo "You should use the commands in this script as a starting"
				echo "point to review your UID-space and manually move the"
				echo "remaining users to any open UID over 1000."
				exit 1
			fi
		done
		sudo dscl . -create "/Users/$name" UniqueID "$next_id"
		echo "   Temporarily moved $name from uid $uid -> $next_id"

	done < <(dscl . list /Users UniqueID | grep _bsdbld | sort -n -k2)
}

change_bsdbld_uids(){
	local existing_gid name next_id user_n

	((next_id=NEW_NIX_FIRST_BUILD_UID))
	((user_n=1))
	name="$(bsd_user_n "$user_n")"
	existing_gid="$(dsclattr "/Groups/bsdbld" "PrimaryGroupID")"

	# we know that we have *some* bsdbld users, but macOS may have
	# already clobbered the first few users if this system has been
	# upgraded

	echo ""
	echo "Step 2: re-create missing early _bsdbld# users."

	until dscl . read "/Users/$name" &>/dev/null; do
		# iterate for a clean ID
		while id_unavailable "$next_id"; do
			((next_id++))
			if ((next_id >= 400)); then
				hit_id_cap
				exit 1
			fi
		done

		re_create_bsdbld_user "$name" "$next_id" "$existing_gid"
		echo "      $name was missing; created with uid: $next_id"

		((user_n++))
		name="$(bsd_user_n "$user_n")"
	done

	echo ""
	echo "Step 3: relocate remaining _bsdbld# UIDs to $next_id+"

	# start at first _bsdbld# not re-created above and increment
	# until _bsdbld<n> doesn't exist
	while dscl . read "/Users/$name" &>/dev/null; do
		# iterate for a clean ID
		while id_unavailable "$next_id"; do
			((next_id++))
			if ((next_id >= 400)); then
				hit_id_cap
				exit 1
			fi
		done

		sudo dscl . -create "/Users/$name" UniqueID "$next_id"
		echo "      $name migrated to uid: $next_id"

		((user_n++))
		name="$(bsd_user_n "$user_n")"
	done

	if ((user_n == 1)); then
		echo "Didn't find _bsdbld1. Perhaps you have single-user Bsd?"
		exit 1
	else
		echo "Migrated $((user_n - 1)) users. If you want to double-check, try:"
		echo "dscl . list /Users UniqueID | grep _bsdbld | sort -n -k2"
	fi
}
needs_migration(){
	local name uid next_id user_n

	((next_id=NEW_NIX_FIRST_BUILD_UID))
	((user_n=1))

	while read -r name uid; do
		expected_name="$(bsd_user_n "$user_n")"
		if [[ "$expected_name" != "$name" ]]; then
			return 0
		fi
		if [[ "$next_id" != "$uid" ]]; then
			return 0
		fi
		((next_id++))
		((user_n++))
	done < <(dscl . list /Users UniqueID | grep _bsdbld | sort -n -k2)
	return 1
}


if any_bsdbld; then
	if needs_migration; then
		echo "Attempting to migrate _bsdbld users."
		temporarily_move_existing_bsdbld_uids
		change_bsdbld_uids
	else
		echo "_bsdbld users already appear to be migrated."
	fi
else
	echo "Didn't find any _bsdbld users. Perhaps you have single-user Bsd?"
	exit 1
fi
