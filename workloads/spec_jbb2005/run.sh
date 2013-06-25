##
## This is an example of what a run sh script might look like
##

date
echo $CLASSPATH
CLASSPATH=./jbb.jar:./check.jar:$CLASSPATH
echo $CLASSPATH
export CLASSPATH

java -fullversion

#java -Xms256m -Xmx256m spec.jbb.JBBmain -propfile SPECjbb.props
#java -Xms512m -Xmx1024m spec.jbb.JBBmain -propfile SPECjbb.props
java -Xms1024m -Xmx1536m spec.jbb.JBBmain -propfile SPECjbb.props
 
date

