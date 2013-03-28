function assert_files
{
for FILE in $* ; do
  if [ ! -e $FILE ] ; then
    echo "failed to create $FILE"
    return 1
  fi
done
return 0
}

function assert_directories
{
for DIR in $* ; do
  if [ ! -d $DIR ] ; then
    echo "failed to create $DIR"
    return 1
  fi
done
return 0
}
