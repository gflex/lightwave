source /var/vmware/lightwave/scripts/common.sh

#Returns list of instance IDs of instances within the ASG
get_node_list() {
    ID_LIST=($(aws autoscaling describe-auto-scaling-groups \
        --auto-scaling-group-names ${ASG_NAME} \
        --region ${EC2_REGION} \
        --query AutoScalingGroups[].Instances[].InstanceId \
        --output text))
    if [ $? -ne 0 ]; then
        exit
    fi
}

#Returns name of latest archive.zip that has been uploaded by any node in the region to S3
get_latest_bkp() {
    LATEST=`aws s3 ls $1/ | grep "archive.zip" | sort | tail -n 1 | awk '{print $4}'`
}

#Chek if the bucket exists. If not, create it
check_or_create_bucket() {
    if aws s3 ls "s3://$BUCKET" 2>&1 | grep -q 'NoSuchBucket'
    then
        logger -t run_backup "Creating bucket "$BUCKET
        aws s3api create-bucket --bucket $BUCKET --region $REGION --create-bucket-configuration LocationConstraint=$REGION
        if [ $? -ne 0 ]; then
            exit
        fi
    fi
}

get_account_id() {
    ACCOUNT_ID=`curl -s http://169.254.169.254/latest/dynamic/instance-identity/document | grep accountId | cut -d ":" -f 2 | cut -d "\"" -f 2`
    if [ $? -ne 0 ]; then
        logger -t backup "Error while getting account id"
        exit
    fi
}

upload_to_cloud() {
    aws s3 cp "$2" "s3://$BUCKET/$1"
    if [ $? -ne 0 ]; then
        return 1
    fi
    return 0
}

#Get list of all files in the backup directory
get_bkp_list() {
    LIST=`aws s3 ls $1/ | grep archive`
    if [ $? -ne 0 ]; then
        exit
    fi
}

delete_file() {
    aws s3 rm s3://$1/$2
}

exit_if_no_tag() {
    aws autoscaling describe-tags --region $EC2_REGION --filters "Name=auto-scaling-group,Values=$ASG_NAME" "Name=key,Values=BACKUP_ENABLED" "Name=Value,Values=1" |grep ResourceId
    if [ $? -ne 0 ]; then
        exit
    fi
}

#argument: lw-dr or post-dr
get_full_path() {
    FULL_PATH="$BUCKET/$1/$ASG_NAME"
}

REGION=$EC2_REGION
get_account_id
BUCKET=dr-$ACCOUNT_ID-$REGION
LW_BKP_PATH=lw-dr/$ASG_NAME
POST_BKP_PATH=post-dr/$ASG_NAME
