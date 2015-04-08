<?php
/* @var $this WSNtableController */
/* @var $model WSNtable */

$this->breadcrumbs=array(
	'Wsntables'=>array('index'),
	$model->NUMSEQ=>array('view','id'=>$model->NUMSEQ),
	'Update',
);

$this->menu=array(
	array('label'=>'List WSNtable', 'url'=>array('index')),
	array('label'=>'Create WSNtable', 'url'=>array('create')),
	array('label'=>'View WSNtable', 'url'=>array('view', 'id'=>$model->NUMSEQ)),
	array('label'=>'Manage WSNtable', 'url'=>array('admin')),
);
?>

<h1>Update WSNtable <?php echo $model->NUMSEQ; ?></h1>

<?php $this->renderPartial('_form', array('model'=>$model)); ?>