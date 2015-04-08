<?php
/* @var $this WSNtableController */
/* @var $model WSNtable */

$this->breadcrumbs=array(
	'Wsntables'=>array('index'),
	$model->NUMSEQ,
);

$this->menu=array(
	array('label'=>'List WSNtable', 'url'=>array('index')),
	array('label'=>'Create WSNtable', 'url'=>array('create')),
	array('label'=>'Update WSNtable', 'url'=>array('update', 'id'=>$model->NUMSEQ)),
	array('label'=>'Delete WSNtable', 'url'=>'#', 'linkOptions'=>array('submit'=>array('delete','id'=>$model->NUMSEQ),'confirm'=>'Are you sure you want to delete this item?')),
	array('label'=>'Manage WSNtable', 'url'=>array('admin')),
);
?>

<h1>View WSNtable #<?php echo $model->NUMSEQ; ?></h1>

<?php $this->widget('zii.widgets.CDetailView', array(
	'data'=>$model,
	'attributes'=>array(
		'NUMSEQ',
		'NODEID',
		'TYPE',
		'TYPENOTE',
		'VALUE',
		'SENDTIME',
		'RECVTIME',
		'LOCALTION',
	),
)); ?>
